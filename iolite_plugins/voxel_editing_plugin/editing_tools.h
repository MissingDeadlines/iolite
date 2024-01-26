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
namespace editing_tools
{

//----------------------------------------------------------------------------//
enum placement_mode_
{
  placement_mode_attach,
  placement_mode_paint,
  placement_mode_erase
};
using placement_mode_t = int32_t;

//----------------------------------------------------------------------------//
enum tool_
{
  tool_modify,
  tool_extrude,
  tool_eyedropper,
  tool_move,
  tool_grass,

  tool_select_box,
  tool_select_wand,
  tool_select_begin = tool_select_box,
  tool_select_end = tool_select_wand + 1
};
using tool_t = int32_t;

//----------------------------------------------------------------------------//
enum tool_shape_
{
  tool_shape_box,
  tool_shape_voxel_box,
  tool_shape_voxel_sphere
};
using tool_shape_t = int32_t;

//----------------------------------------------------------------------------//
enum region_type_
{
  region_type_color,
  region_type_region,
  region_type_region_color
};
using region_type_t = int32_t;

//----------------------------------------------------------------------------//
enum face_type_
{
  face_type_region,
  face_type_region_color
};
using face_type_t = int32_t;

//----------------------------------------------------------------------------//
struct tool_parameters_t
{
  int32_t extent{1};
  float density{1.0f};
  tool_t tool{tool_modify};
  placement_mode_t placement_mode{placement_mode_attach};
  tool_shape_t tool_shape{tool_shape_voxel_box};
  bool tool_shape_is_3d{true};

  region_neighborhood_t region_neighborhood{region_neighborhood_6};
  region_type_t region_type{region_type_region};
  float region_fuziness{0.0f};

  region_neighborhood_t face_neighborhood{region_neighborhood_6};
  face_type_t face_type{face_type_region};
  bool face_palette_fill{false};
  float face_fuzziness{0.0f};

  palette_range_t palette_range;
  bool mirror_x{false}, mirror_y{false}, mirror_z{false};
  sparse_volume_t selection, clipboard;

  float tool_grass_density{0.25f};
};

//----------------------------------------------------------------------------//
static auto is_left_mouse_buttom_clicked() -> bool
{
  return io_input_system->get_key_state(io_input_key_mouse_left, 0u) ==
             io_input_key_state_clicked &&
         !ImGui::GetIO().WantCaptureMouse;
}

//----------------------------------------------------------------------------//
static auto is_left_mouse_buttom_pressed() -> bool
{
  return io_input_system->get_key_state(io_input_key_mouse_left, 0u) ==
             io_input_key_state_pressed &&
         !ImGui::GetIO().WantCaptureMouse;
}

//----------------------------------------------------------------------------//
static void mirror(io_ref_t shape, const tool_parameters_t& params,
                   sparse_volume_t& voxels)
{
  uint8_t axis_mask = (uint8_t)params.mirror_x;
  axis_mask |= (uint8_t)params.mirror_y << 1u;
  axis_mask |= (uint8_t)params.mirror_z << 2u;

  voxels.mirror(shape, axis_mask);
}

//----------------------------------------------------------------------------//
static bool trace_volume(io_ref_t shape, const tool_parameters_t& params,
                         io_component_voxel_shape_raycast_result_t& result)
{
  const auto dim = io_component_voxel_shape->get_dim(shape);

  io_vec3_t o, d;
  io_world->calc_mouse_ray(&o, &d);

  // Raycast against shape's voxels
  bool hit = io_component_voxel_shape->raycast(shape, o, d, FLT_MAX, &result);
  bool block = false;
  if (hit)
  {
    bool shift_to_adjacent_voxel = false;
    shift_to_adjacent_voxel |= params.tool == tool_modify &&
                               params.placement_mode == placement_mode_attach;

    if (shift_to_adjacent_voxel)
    {
      io_ivec3_t coord = {result.coord.x, result.coord.y, result.coord.z};

      if (glm::abs(result.normal_local.x) > 0.1f)
        coord.x += glm::sign(result.normal_local.x);
      else if (glm::abs(result.normal_local.y) > 0.1f)
        coord.y += glm::sign(result.normal_local.y);
      else if (glm::abs(result.normal_local.z) > 0.1f)
        coord.z += glm::sign(result.normal_local.z);

      hit = coord.x >= 0u && coord.y >= 0u && coord.z >= 0 && coord.x < dim.x &&
            coord.y < dim.y && coord.z < dim.z;
      block = !hit;
      result.coord = {(uint8_t)coord.x, (io_uint8_t)coord.y, (uint8_t)coord.z};
    }
  }

  bool trace_bounds = false;
  trace_bounds |= params.tool == tool_modify &&
                  params.placement_mode == placement_mode_attach;
  trace_bounds |= params.tool == tool_select_box;

  // Raycast against bounds
  if (!hit && !block && trace_bounds)
  {
    hit = io_component_voxel_shape->raycast_bounds(shape, o, d, FLT_MAX, true,
                                                   &result);
  }

  return hit;
}

//----------------------------------------------------------------------------//
static void handle_tool_voxel(io_ref_t shape, const tool_parameters_t& params)
{
  auto palette = io_component_voxel_shape->get_palette(shape);

  if (!io_base->ref_is_valid(palette))
    return;

  const auto dim = io_component_voxel_shape->get_dim(shape);

  sparse_volume_t voxels;

  io_component_voxel_shape_raycast_result_t result;
  const bool hit = trace_volume(shape, params, result);
  if (hit)
  {
    common::srand(result.coord);

    // Easier to tweak
    const float density2 = params.density * params.density;

    if (params.tool_shape == tool_shape_voxel_box)
    {
      auto offset = io_ivec3_t{result.coord.x, result.coord.y, result.coord.z};
      auto extent = io_ivec3_t{params.extent, params.extent, params.extent};

      if (!params.tool_shape_is_3d)
      {
        if (glm::abs(result.normal.x) > 1e-6f)
        {
          extent.x = 1;

          offset.y -= glm::floor(extent.y * 0.5f);
          offset.z -= glm::floor(extent.z * 0.5f);
        }
        else if (glm::abs(result.normal.y) > 1e-6f)
        {
          extent.y = 1;

          offset.x -= glm::floor(extent.x * 0.5f);
          offset.z -= glm::floor(extent.z * 0.5f);
        }
        else if (glm::abs(result.normal.z) > 1e-6f)
        {
          extent.z = 1;

          offset.x -= glm::floor(extent.x * 0.5f);
          offset.y -= glm::floor(extent.y * 0.5f);
        }
      }
      else
      {
        const bool invert_shift =
            params.placement_mode == placement_mode_attach;

        if (glm::abs(result.normal.x) > 1e-6f)
        {
          float shift = glm::step(0.0f, result.normal.x);
          shift = invert_shift ? (1.0f - shift) : shift;

          offset.x -= shift * (extent.x - 1);
          offset.y -= glm::floor(extent.y * 0.5f);
          offset.z -= glm::floor(extent.z * 0.5f);
        }
        else if (glm::abs(result.normal.y) > 1e-6f)
        {
          float shift = glm::step(0.0f, result.normal.y);
          shift = invert_shift ? (1.0f - shift) : shift;

          offset.x -= glm::floor(extent.x * 0.5f);
          offset.y -= shift * (extent.y - 1);
          offset.z -= glm::floor(extent.z * 0.5f);
        }
        else if (glm::abs(result.normal.z) > 1e-6f)
        {
          float shift = glm::step(0.0f, result.normal.z);
          shift = invert_shift ? (1.0f - shift) : shift;

          offset.x -= glm::floor(extent.x * 0.5f);
          offset.y -= glm::floor(extent.y * 0.5f);
          offset.z -= shift * (extent.z - 1);
        }
      }

      for (int32_t z = 0u; z < extent.z; ++z)
        for (int32_t y = 0u; y < extent.y; ++y)
          for (int32_t x = 0u; x < extent.x; ++x)
          {
            const float r = common::rand_float(0.0f, 1.0f);
            if (density2 < r)
              continue;

            const uint8_t value =
                params.placement_mode != placement_mode_erase
                    ? (params.palette_range.get_random_palette_index() + 1u)
                    : 0u;
            voxels.set(offset.x + x, offset.y + y, offset.z + z, value, dim);
          }
    }
    else if (params.tool_shape == tool_shape_voxel_sphere)
    {
      auto extent = io_ivec3_t{params.extent, params.extent, params.extent};
      const auto radius = params.extent * 0.5f;
      auto offset = io_ivec3_t{(int32_t)glm::ceil(result.coord.x - radius),
                               (int32_t)glm::ceil(result.coord.y - radius),
                               (int32_t)glm::ceil(result.coord.z - radius)};

      if (!params.tool_shape_is_3d)
      {
        if (glm::abs(result.normal.x) > 1e-6f)
        {
          extent.x = 1;
          offset.x = result.coord.x;
        }
        else if (glm::abs(result.normal.y) > 1e-6f)
        {
          extent.y = 1;
          offset.y = result.coord.y;
        }
        else if (glm::abs(result.normal.z) > 1e-6f)
        {
          extent.z = 1;
          offset.z = result.coord.z;
        }
      }

      const glm::vec3 radius_final = {extent.x * 0.5f, extent.y * 0.5f,
                                      extent.z * 0.5f};

      for (uint32_t z = 0u; z < extent.z; ++z)
        for (uint32_t y = 0u; y < extent.y; ++y)
          for (uint32_t x = 0u; x < extent.x; ++x)
          {
            const float r = common::rand_float(0.0f, 1.0f);
            if (density2 < r)
              continue;

            const auto coord =
                (glm::vec3(x, y, z) - radius_final + 0.5f) / radius;
            if (glm::length(coord) >= 1.0f)
              continue;

            uint8_t palette_index =
                params.palette_range.get_random_palette_index() + 1u;
            if (params.placement_mode == placement_mode_erase)
              palette_index = 0u;

            voxels.set(offset.x + x, offset.y + y, offset.z + z, palette_index,
                       dim);
          }
    }
  }

  // Remove non-solid voxels?
  const bool solid_voxels_only =
      params.placement_mode == placement_mode_paint ||
      params.placement_mode == placement_mode_erase;
  if (solid_voxels_only)
    voxels.remove_voxels<remove_mode_non_solid>(shape);
  else
    voxels.remove_voxels<remove_mode_solid>(shape);

  // Apply mirroring (if any)
  mirror(shape, params, voxels);

  static sparse_volume_t paint_volume;
  if (is_left_mouse_buttom_pressed())
  {
    paint_volume.add(voxels, dim);
  }
  else if (!is_left_mouse_buttom_pressed() && !paint_volume.empty())
  {
    const auto current_palette_index =
        io_component_voxel_shape->get(shape, result.coord);

    // Undo/redo
    io_editor->push_undo_redo_state_for_entity(
        ICON_FA_PEN_TO_SQUARE "   Brush Shape",
        io_component_voxel_shape->base.get_entity(shape), false);

    // Apply the changes to the shape
    paint_volume.apply(shape);
    // Commit the changes
    io_component_voxel_shape->commit_snapshot(shape);
    io_component_voxel_shape->voxelize(shape);

    paint_volume.clear();
  }

  if (paint_volume.empty())
  {
    voxels.cull_for_draw(shape);
    voxels.draw(shape);
  }
  else
  {
    paint_volume.cull_for_draw(shape);
    paint_volume.draw(shape, true);
  }
}

//----------------------------------------------------------------------------//
static void handle_tool_select_wand(io_ref_t shape, tool_parameters_t& params)
{
  auto palette = io_component_voxel_shape->get_palette(shape);

  if (!io_base->ref_is_valid(palette))
    return;

  const auto dim = io_component_voxel_shape->get_dim(shape);

  sparse_volume_t voxels;

  io_component_voxel_shape_raycast_result_t result;
  const bool hit = trace_volume(shape, params, result);

  if (hit)
  {
    common::srand(result.coord);

    if (params.region_type != region_type_color)
    {
      voxels.add_region(shape, result.coord, params.region_neighborhood, 0xFFu,
                        {}, params.region_type == region_type_region_color,
                        palette, params.face_fuzziness);
    }
    else
    {

      voxels.add_by_palette_index(shape, result.coord);
    }

    if (is_left_mouse_buttom_clicked())
    {
      if (!ImGui::GetIO().KeyShift)
        params.selection = voxels;
      else
        params.selection.add(voxels, dim);
    }
  }
  else
  {
    if (is_left_mouse_buttom_clicked())
      params.selection.clear();
  }

  voxels.cull_for_draw(shape);
  voxels.draw(shape, false, true);
}

//----------------------------------------------------------------------------//
inline static auto closest_point_on_axis(const io_vec3_t& ray_o,
                                         const io_vec3_t& ray_d,
                                         const io_vec3_t& axis_o,
                                         const io_vec3_t& axis_d) -> io_vec3_t
{
  // Nearest point between two skew lines
  const auto p0 = io_cvt(axis_o);
  const auto p1 = io_cvt(ray_o);
  const auto d0 = io_cvt(axis_d);
  const auto d1 = io_cvt(ray_d);

  const auto n = glm::cross(d0, d1);
  const auto n1 = glm::cross(d1, n);
  const auto c0 = p0 + (glm::dot(p1 - p0, n1)) / glm::dot(d0, n1) * d0;

  return io_cvt(c0);
}

//----------------------------------------------------------------------------//
inline static auto closest_point_on_ray(const io_vec3_t& ray_o,
                                        const io_vec3_t& ray_d,
                                        const io_vec3_t& axis_o,
                                        const io_vec3_t& axis_d) -> io_vec3_t
{
  // Nearest point between two skew lines
  const auto p0 = io_cvt(axis_o);
  const auto p1 = io_cvt(ray_o);
  const auto d0 = io_cvt(axis_d);
  const auto d1 = io_cvt(ray_d);

  const auto n = glm::cross(d0, d1);
  const auto n0 = glm::cross(d0, n);
  const auto c1 = p1 + (glm::dot(p0 - p1, n0)) / glm::dot(d1, n0) * d1;

  return io_cvt(c1);
}

//----------------------------------------------------------------------------//
typedef void (*extrude_function)(io_ref_t shape, tool_parameters_t& params,
                                 uint32_t num_steps, uint32_t axis_index,
                                 int32_t axis_sign, int32_t skip_self,
                                 const sparse_volume_t& voxels,
                                 sparse_volume_t& voxels_extruded);

//----------------------------------------------------------------------------//
static void handle_tool_extrude(io_ref_t shape, tool_parameters_t& params,
                                extrude_function extrude)
{
  auto palette = io_component_voxel_shape->get_palette(shape);

  if (!io_base->ref_is_valid(palette))
    return;

  const auto dim = io_component_voxel_shape->get_dim(shape);

  static sparse_volume_t voxels, voxels_extruded;
  static io_vec3_t drag_normal, drag_origin;
  static io_u8vec3_t drag_seed_coord;

  io_component_voxel_shape_raycast_result_t result;
  const bool hit = trace_volume(shape, params, result);
  static bool dragging = false;

  if (hit)
  {
    if (!dragging)
    {
      uint8_t axis_flags = 0xFFu;
      io_ivec3_t exclude_dir = {};

      if (glm::abs(result.normal_local.x) > 0.1f)
      {
        axis_flags &= ~0x01u;
        exclude_dir.x = glm::sign(result.normal_local.x);
      }
      else if (glm::abs(result.normal_local.y) > 0.1f)
      {
        axis_flags &= ~0x02u;
        exclude_dir.y = glm::sign(result.normal_local.y);
      }
      else if (glm::abs(result.normal_local.z) > 0.1f)
      {
        axis_flags &= ~0x04u;
        exclude_dir.z = glm::sign(result.normal_local.z);
      }

      voxels.clear();
      voxels.add_region(shape, result.coord, params.face_neighborhood,
                        axis_flags, exclude_dir,
                        params.face_type == face_type_region_color, palette,
                        params.face_fuzziness);
      if (params.face_palette_fill)
        voxels = voxels.prepare_fill(shape, params.palette_range, true);

      drag_normal = result.normal_local;
      if (params.placement_mode == placement_mode_erase ||
          params.placement_mode == placement_mode_paint)
      {
        drag_normal = {-drag_normal.x, -drag_normal.y, -drag_normal.z};
      }

      drag_origin = {result.coord.x + 0.5f, result.coord.y + 0.5f,
                     result.coord.z + 0.5f};
      drag_seed_coord = result.coord;

      if (is_left_mouse_buttom_pressed())
        dragging = true;
    }
  }
  else
  {
    if (!dragging)
      voxels.clear();
  }

  if (dragging)
  {
    io_vec3_t o, d;
    io_world->calc_mouse_ray(&o, &d);

    const auto entity = io_component_voxel_shape->base.get_entity(shape);
    const auto node = io_component_node->base.get_component_for_entity(entity);

    const auto ray_origin = io_component_voxel_shape->to_local_space(shape, o);
    const auto ray_dir = io_component_node->to_local_space_direction(node, d);

    const auto an_abs = glm::abs(io_cvt(drag_normal));
    const uint32_t axis_index =
        (an_abs.y > 0.1f) ? 1u : ((an_abs.z > 0.1f) ? 2u : 0u);
    const int32_t axis_sign = glm::sign(drag_normal.data[axis_index]);

    const auto cp =
        closest_point_on_axis(ray_origin, ray_dir, drag_origin, drag_normal);
    const int32_t drag = cp.data[axis_index] - drag_origin.data[axis_index];

    const uint32_t num_steps =
        axis_sign > 0 ? glm::max(drag, 0) : -glm::min(drag, 0);
    const int32_t skip_self = params.placement_mode == placement_mode_attach;
    const auto dim = io_component_voxel_shape->get_dim(shape);

    common::srand(drag_seed_coord);
    voxels_extruded.clear();
    extrude(shape, params, num_steps, axis_index, axis_sign, skip_self, voxels,
            voxels_extruded);
    mirror(shape, params, voxels_extruded);

    if (params.placement_mode == placement_mode_erase)
    {
      voxels_extruded = voxels_extruded.prepare_erase(shape);
    }
    else if (params.placement_mode == placement_mode_paint)
    {
      voxels_extruded =
          voxels_extruded.prepare_fill(shape, params.palette_range);
      voxels_extruded.remove_voxels<remove_mode_non_solid>(shape);
    }

    if (!is_left_mouse_buttom_pressed())
    {
      // Undo/redo
      io_editor->push_undo_redo_state_for_entity(
          ICON_FA_PEN_TO_SQUARE "   Extrude Shape",
          io_component_voxel_shape->base.get_entity(shape), false);

      voxels_extruded.apply(shape);
      io_component_voxel_shape->commit_snapshot(shape);

      voxels_extruded.clear();
      voxels.clear();
      dragging = false;
    }
  }

  auto voxels_to_draw = voxels;
  mirror(shape, params, voxels_to_draw);
  voxels_to_draw.cull_for_draw(shape);
  voxels_to_draw.draw(shape, false, true);

  voxels_extruded.cull_for_draw(shape);
  voxels_extruded.draw(shape, true, false);
}

//----------------------------------------------------------------------------//
static void handle_tool_extrude(io_ref_t shape, tool_parameters_t& params)
{
  handle_tool_extrude(
      shape, params,
      [](io_ref_t shape, tool_parameters_t& params, uint32_t num_steps,
         uint32_t axis_index, int32_t axis_sign, int32_t skip_self,
         const sparse_volume_t& voxels, sparse_volume_t& voxels_extruded) {
        const auto dim = io_component_voxel_shape->get_dim(shape);

        for (uint32_t i = 0u; i < num_steps; ++i)
        {
          io_ivec3_t offset = {};
          offset.data[axis_index] = axis_sign * i + skip_self * axis_sign;
          voxels_extruded.add(voxels, dim, offset);
        }
      });
}

//----------------------------------------------------------------------------//
static void handle_tool_grass(io_ref_t shape, tool_parameters_t& params)
{
  handle_tool_extrude(
      shape, params,
      [](io_ref_t shape, tool_parameters_t& params, uint32_t num_steps,
         uint32_t axis_index, int32_t axis_sign, int32_t skip_self,
         const sparse_volume_t& voxels, sparse_volume_t& voxels_extruded) {
        if (num_steps < 1u)
          return;

        const auto dim = io_component_voxel_shape->get_dim(shape);

        for (const auto entry : voxels.entries)
        {
          uint8_t x, y, z, palette_index;
          sparse_volume_t::unpack(entry.data, x, y, z, &palette_index);

          const float r = common::rand_float(0.0f, 1.0f);
          const float h = common::rand_float(0.0f, 1.0f);

          if (params.face_palette_fill)
            palette_index = params.palette_range.get_random_palette_index() + 1;

          if (r > params.tool_grass_density)
            continue;

          const uint32_t steps = (num_steps - 1u) * h + 1u;

          for (uint32_t i = 0u; i < steps; ++i)
          {
            io_ivec3_t coord = {x, y, z};
            coord.data[axis_index] += axis_sign * i + skip_self * axis_sign;

            voxels_extruded.set(coord, palette_index, dim);
          }
        }
      });
}

//----------------------------------------------------------------------------//
static void handle_tool_move(io_ref_t shape, tool_parameters_t& params)
{
  auto palette = io_component_voxel_shape->get_palette(shape);

  if (!io_base->ref_is_valid(palette))
    return;

  const auto dim = io_component_voxel_shape->get_dim(shape);

  static sparse_volume_t voxels_to_move;
  sparse_volume_t voxels_to_draw;
  static io_vec3_t drag_normal, drag_tangent, drag_origin;

  io_component_voxel_shape_raycast_result_t result;
  const bool hit = trace_volume(shape, params, result);
  static bool dragging = false;

  if (hit)
  {
    common::srand(result.coord);

    if (is_left_mouse_buttom_pressed() && !dragging)
    {
      uint8_t axis_flags = 0xFFu;
      io_ivec3_t exclude_dir = {};

      if (glm::abs(result.normal_local.x) > 0.1f)
      {
        drag_normal = {0.0f, 1.0f, 0.0f};
        drag_tangent = {0.0f, 0.0f, 1.0f};
      }
      else if (glm::abs(result.normal_local.y) > 0.1f)
      {
        drag_normal = {1.0f, 0.0f, 0.0f};
        drag_tangent = {0.0f, 0.0f, 1.0f};
      }
      else if (glm::abs(result.normal_local.z) > 0.1f)
      {
        drag_normal = {1.0f, 0.0f, 0.0f};
        drag_tangent = {0.0f, 1.0f, 0.0f};
      }

      drag_origin = {result.coord.x + 0.5f, result.coord.y + 0.5f,
                     result.coord.z + 0.5f};

      // Copy selection
      voxels_to_move.add(params.selection, dim);
      voxels_to_move.update_from_shape(shape);

      params.selection.clear();
      // Remove voxels
      auto change = voxels_to_move.prepare_erase(shape);

      // Undo/redo
      io_editor->push_undo_redo_state_for_entity(
          ICON_FA_PEN_TO_SQUARE "   Move Shape",
          io_component_voxel_shape->base.get_entity(shape), false);

      change.apply(shape);
      io_component_voxel_shape->commit_snapshot(shape);

      dragging = true;
    }
  }

  if (dragging)
  {
    io_vec3_t o, d;
    io_world->calc_mouse_ray(&o, &d);

    const auto entity = io_component_voxel_shape->base.get_entity(shape);
    const auto node = io_component_node->base.get_component_for_entity(entity);

    const auto ray_origin = io_component_voxel_shape->to_local_space(shape, o);
    const auto ray_dir = io_component_node->to_local_space_direction(node, d);

    const auto cpn0 =
        closest_point_on_axis(ray_origin, ray_dir, drag_origin, drag_normal);
    const auto cpn1 =
        closest_point_on_ray(ray_origin, ray_dir, drag_origin, drag_normal);
    const float d0 = glm::distance2(io_cvt(cpn0), io_cvt(cpn1));

    const auto cpt0 =
        closest_point_on_axis(ray_origin, ray_dir, drag_origin, drag_tangent);
    const auto cpt1 =
        closest_point_on_ray(ray_origin, ray_dir, drag_origin, drag_tangent);
    const float d1 = glm::distance2(io_cvt(cpt0), io_cvt(cpt1));

    auto axis_normal = drag_normal;
    auto axis_point = cpn0;
    if (d1 < d0)
    {
      axis_normal = drag_tangent;
      axis_point = cpt0;
    }

    const auto an_abs = glm::abs(io_cvt(axis_normal));
    const uint32_t axis_idx =
        (an_abs.y > 0.1f) ? 1u : ((an_abs.z > 0.1f) ? 2u : 0u);

    int32_t drag = axis_point.data[axis_idx] - drag_origin.data[axis_idx];

    voxels_to_draw.add(voxels_to_move, dim,
                       {int32_t(axis_normal.x * drag),
                        int32_t(axis_normal.y * drag),
                        int32_t(axis_normal.z * drag)});

    if (!is_left_mouse_buttom_pressed())
    {
      // Move voxels
      auto change = voxels_to_draw;
      change.update_from_shape(shape);

      // Undo/redo
      io_editor->push_undo_redo_state_for_entity(
          ICON_FA_PEN_TO_SQUARE "   Move Shape",
          io_component_voxel_shape->base.get_entity(shape), false);

      voxels_to_draw.apply(shape);
      io_component_voxel_shape->commit_snapshot(shape);

      // Update selection
      params.selection = voxels_to_draw;

      // We're done
      voxels_to_draw.clear();
      voxels_to_move.clear();
      dragging = false;
    }
  }

  voxels_to_draw.cull_for_draw(shape);
  voxels_to_draw.draw(shape, true, false);
}

//----------------------------------------------------------------------------//
static void handle_tool_eyedropper(io_ref_t shape, tool_parameters_t& params)
{
  auto palette = io_component_voxel_shape->get_palette(shape);

  if (!io_base->ref_is_valid(palette))
    return;

  const auto dim = io_component_voxel_shape->get_dim(shape);

  sparse_volume_t voxels;

  io_component_voxel_shape_raycast_result_t result;
  const bool hit = trace_volume(shape, params, result);
  const bool dragging = is_left_mouse_buttom_pressed();

  if (hit)
  {
    common::srand(result.coord);

    voxels.set(result.coord, 0u, dim);
    voxels.update_from_shape(shape);

    if (is_left_mouse_buttom_clicked())
    {
      uint8_t x, y, z, palette_index;
      sparse_volume_t::unpack(voxels.entries.front().data, x, y, z,
                              &palette_index);
      params.palette_range = {io_uint8_t(palette_index - 1u),
                              io_uint8_t(palette_index - 1u)};
    }
  }

  voxels.cull_for_draw(shape);
  voxels.draw(shape, false, true);
}

//----------------------------------------------------------------------------//
static void handle_tool_box(io_ref_t shape, tool_parameters_t& params)
{
  auto palette = io_component_voxel_shape->get_palette(shape);

  if (!io_base->ref_is_valid(palette))
    return;

  const auto dim = io_component_voxel_shape->get_dim(shape);

  static sparse_volume_t voxels;

  io_component_voxel_shape_raycast_result_t result;
  const bool hit = trace_volume(shape, params, result);
  const bool is_selection = params.tool == tool_select_box;
  const bool should_erase = params.tool == tool_modify &&
                            params.placement_mode == placement_mode_erase;

  if (hit)
  {
    common::srand(result.coord);

    static io_ivec3_t box_min{}, box_max{};
    static bool dragging = false;

    if (!is_left_mouse_buttom_pressed())
    {
      if (dragging)
      {
        if (is_selection)
        {
          if (!ImGui::GetIO().KeyShift)
            params.selection = voxels;
          else
            params.selection.add(voxels, dim);
        }
        else
        {
          auto change = voxels;
          change.update_from_shape(shape);

          // Undo/redo
          io_editor->push_undo_redo_state_for_entity(
              ICON_FA_PEN_TO_SQUARE "   Box Shape",
              io_component_voxel_shape->base.get_entity(shape), false);

          voxels.apply(shape);
          io_component_voxel_shape->commit_snapshot(shape);
        }

        dragging = false;
      }

      box_min = box_max = {result.coord.x, result.coord.y, result.coord.z};
    }
    else
    {
      dragging = true;
      box_max = {result.coord.x, result.coord.y, result.coord.z};
    }

    int32_t min_x, min_y, min_z, max_x, max_y, max_z;

#define FIND_MIN_MAX(_axis)                                                    \
  if (box_min._axis <= box_max._axis)                                          \
  {                                                                            \
    min_##_axis = box_min._axis;                                               \
    max_##_axis = box_max._axis;                                               \
  }                                                                            \
  else                                                                         \
  {                                                                            \
    min_##_axis = box_max._axis;                                               \
    max_##_axis = box_min._axis;                                               \
  }

    FIND_MIN_MAX(x);
    FIND_MIN_MAX(y);
    FIND_MIN_MAX(z);

#undef FIND_MIN_MAX

    voxels.clear();
    for (int32_t z = min_z; z <= max_z; ++z)
      for (int32_t y = min_y; y <= max_y; ++y)
        for (int32_t x = min_x; x <= max_x; ++x)
        {
          const uint8_t palette_index =
              (!should_erase && !is_selection)
                  ? params.palette_range.get_random_palette_index() + 1u
                  : 0u;
          voxels.set(x, y, z, palette_index, dim);
        }
  }
  else
  {
    if (is_selection && is_left_mouse_buttom_clicked())
      params.selection.clear();
  }

  // Remove non-solid voxels?
  bool solid_voxels_only = is_selection;
  solid_voxels_only |= params.tool == tool_modify &&
                       params.placement_mode == placement_mode_erase;
  solid_voxels_only |= params.tool == tool_modify &&
                       params.placement_mode == placement_mode_paint;

  // Apply mirroring (if any)
  mirror(shape, params, voxels);

  if (solid_voxels_only)
    voxels.remove_voxels<remove_mode_non_solid>(shape);
  else
    voxels.remove_voxels<remove_mode_solid>(shape);

  if (is_selection)
    voxels.update_from_shape(shape);

  voxels.cull_for_draw(shape);
  voxels.draw(shape, false, is_selection);
}

//----------------------------------------------------------------------------//
static void handle_tool(tool_parameters_t& params)
{
  const auto entity = io_editor->get_first_selected_entity();
  if (!io_base->ref_is_valid(entity))
    return;

  const auto shape =
      io_component_voxel_shape->base.get_component_for_entity(entity);

  if (!io_base->ref_is_valid(shape))
    return;

  if (params.tool == tool_modify)
  {
    if (params.tool_shape != tool_shape_box)
      handle_tool_voxel(shape, params);
    else
      handle_tool_box(shape, params);
  }
  else if (params.tool == tool_extrude)
  {
    handle_tool_extrude(shape, params);
  }
  else if (params.tool == tool_eyedropper)
  {
    handle_tool_eyedropper(shape, params);
  }
  else if (params.tool == tool_select_box)
  {
    handle_tool_box(shape, params);
  }
  else if (params.tool == tool_select_wand)
  {
    handle_tool_select_wand(shape, params);
  }
  else if (params.tool == tool_move)
  {
    handle_tool_move(shape, params);
  }
  else if (params.tool == tool_grass)
  {
    handle_tool_grass(shape, params);
  }

  if (!params.selection.empty())
  {
    params.selection.cull_for_draw(shape);
    params.selection.draw(shape, false, true);
  }
}

} // namespace editing_tools