// MIT License
//
// Copyright (c) 2023 Missing Deadlines (Benjamin Wrensch)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "common.h"
#include "sparse_volume.h"

//----------------------------------------------------------------------------//
namespace editing
{

//----------------------------------------------------------------------------//
enum bool_op_
{
  bool_op_subtraction,
  bool_op_replacement,
  bool_op_intersection,
  bool_op_union
};
typedef uint8_t bool_op_t;

//----------------------------------------------------------------------------//
static void global_full(io_ref_t shape, const palette_range_t& range)
{
  // Undo/redo
  io_editor->push_undo_redo_state_for_entity(
      "Edit Voxel Shape", io_component_voxel_shape->base.get_entity(shape),
      false);

  auto write_ptr = io_component_voxel_shape->get_voxel_data(shape);
  auto dim = io_component_voxel_shape->get_dim(shape);
  const auto num_voxels = dim.x * dim.y * dim.z;

  for (uint32_t z = 0u; z < dim.z; ++z)
  {
    for (uint32_t y = 0u; y < dim.y; ++y)
    {
      for (uint32_t x = 0u; x < dim.x; ++x)
      {
        const auto value = range.get_random_palette_index() + 1u;
        *write_ptr++ = value;
      }
    }
  }

  io_component_voxel_shape->commit_snapshot(shape);
}

//----------------------------------------------------------------------------//
static void global_fill(io_ref_t shape, const palette_range_t& range)
{
  // Undo/redo
  io_editor->push_undo_redo_state_for_entity(
      "Edit Voxel Shape", io_component_voxel_shape->base.get_entity(shape),
      false);

  auto write_ptr = io_component_voxel_shape->get_voxel_data(shape);
  const auto dim = io_component_voxel_shape->get_dim(shape);
  const auto num_voxels = dim.x * dim.y * dim.z;

  for (uint32_t z = 0u; z < dim.z; ++z)
  {
    for (uint32_t y = 0u; y < dim.y; ++y)
    {
      for (uint32_t x = 0u; x < dim.x; ++x)
      {
        const auto value = range.get_random_palette_index() + 1u;
        if (*write_ptr != 0u)
          *write_ptr = value;
        ++write_ptr;
      }
    }
  }

  io_component_voxel_shape->commit_snapshot(shape);
}

//----------------------------------------------------------------------------//
static void global_invert(io_ref_t shape, const palette_range_t& range)
{
  // Undo/redo
  io_editor->push_undo_redo_state_for_entity(
      "Edit Voxel Shape", io_component_voxel_shape->base.get_entity(shape),
      false);

  auto write_ptr = io_component_voxel_shape->get_voxel_data(shape);
  const auto dim = io_component_voxel_shape->get_dim(shape);
  const auto num_voxels = dim.x * dim.y * dim.z;

  for (uint32_t z = 0u; z < dim.z; ++z)
  {
    for (uint32_t y = 0u; y < dim.y; ++y)
    {
      for (uint32_t x = 0u; x < dim.x; ++x)
      {
        const auto value = range.get_random_palette_index() + 1u;
        if (*write_ptr != 0u)
          *write_ptr++ = 0u;
        else if (*write_ptr != value)
          *write_ptr++ = value;
      }
    }
  }

  io_component_voxel_shape->commit_snapshot(shape);
}

//----------------------------------------------------------------------------//
static void global_erase(io_ref_t shape) { global_full(shape, {255u}); };

//----------------------------------------------------------------------------//
template <bool_op_t op>
static void global_boolean(io_ref_t shape, io_ref_t op_shape)
{
  // Undo/redo
  io_editor->push_undo_redo_state_for_entity(
      "Edit Voxel Shape", io_component_voxel_shape->base.get_entity(shape),
      false);

  const auto dim = io_cvt(io_component_voxel_shape->get_dim(shape));
  const auto dim_op = io_cvt(io_component_voxel_shape->get_dim(op_shape));
  auto data = io_component_voxel_shape->get_voxel_data(shape);
  auto data_op = io_component_voxel_shape->get_voxel_data(op_shape);

  for (uint32_t z = 0u; z < dim.z; ++z)
  {
    for (uint32_t y = 0u; y < dim.y; ++y)
    {
      for (uint32_t x = 0u; x < dim.x; ++x)
      {
        const auto pos_ws = io_component_voxel_shape->to_world_space(
            shape, {(io_float32_t)x, (io_float32_t)y, (io_float32_t)z});
        const auto pos_op =
            io_cvt(io_component_voxel_shape->to_local_coord(op_shape, pos_ws));
        const uint32_t index = x + y * dim.x + z * dim.x * dim.y;

        auto val_op = 0x0u;
        if (glm::all(glm::greaterThanEqual(pos_op, glm::ivec3(0))) &&
            glm::all(glm::lessThan(pos_op, glm::ivec3(dim_op))))
        {
          const uint32_t index_op =
              pos_op.x + pos_op.y * dim_op.x + pos_op.z * dim_op.x * dim_op.y;
          val_op = data_op[index_op];
        }
        const auto val = data[index];

        if constexpr (op == bool_op_replacement)
        {
          if (val == 0x0u || val_op == 0x0u)
            continue;

          data[index] = val_op;
        }
        else if constexpr (op == bool_op_subtraction)
        {
          if (val == 0x0u || val_op == 0x0u)
            continue;

          data[index] = 0x0u;
        }
        else if constexpr (op == bool_op_union)
        {
          if (val_op == 0x0u)
            continue;

          data[index] = val_op;
        }
        else if constexpr (op == bool_op_intersection)
        {
          if (val == 0x0u || val_op != 0x0u)
            continue;

          data[index] = 0x0u;
        }
      }
    }
  }

  io_component_voxel_shape->commit_snapshot(shape);
}

} // namespace editing