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
#include "iolite_api.h"

// STL
#include <chrono>

//----------------------------------------------------------------------------//
template <bool UseMortonEncoding = false> struct sparse_occupancy_mask_t
{
  struct mask_t
  {
    inline mask_t() { data.resize(32768u); } // 32^3

    std::vector<bool> data;
  };

  template <typename T> inline auto set(const T& coord, bool occupied) -> bool
  {
    auto* mask = get_mask(coord);
    const auto mask_index = calc_mask_index(coord);

    const bool was_occupied = mask->data[mask_index];
    mask->data[mask_index] = occupied;

    return was_occupied != occupied;
  };

  template <typename T> inline auto get(const T& coord) -> bool
  {
    auto* mask = get_mask<T, false>(coord);
    if (nullptr == mask)
      return false;
    return mask->data[calc_mask_index(coord)];
  };

  inline auto clear() -> void { cells.clear(); }

private:
  template <typename T, bool Create = true>
  inline auto get_mask(const T& coord) -> mask_t*
  {
    const auto cell_index = calc_cell_index(coord);
    if constexpr (Create)
    {
      if (cell_index >= cells.size())
        cells.resize(cell_index + 1u);
    }
    else
    {
      if (cell_index >= cells.size())
        return nullptr;
    }
    return &cells[cell_index];
  }

  template <typename T>
  inline static auto calc_cell_index(const T& coord) -> uint32_t
  {
    // Quantize to 32^3 cells
    const io_uvec3_t coord_quant = {(uint32_t)coord.x >> 5u,
                                    (uint32_t)coord.y >> 5u,
                                    (uint32_t)coord.z >> 5u};
    return coord_quant.x + (coord_quant.y << 3u) +
           (coord_quant.z << 6u); // Maximum of 8^3 cells in a 256^3 global grid
  }

  template <typename T>
  inline static auto calc_mask_index(const T& coord) -> uint32_t
  {
    if constexpr (UseMortonEncoding)
    {
      return morton::encode_5b(
          io_uvec3_t{(uint32_t)coord.x, (uint32_t)coord.y, (uint32_t)coord.z});
    }
    else
    {
      // Wrap in a single 32^3 cell
      const io_uvec3_t coord_mask = {(uint32_t)coord.x & 31u,
                                     (uint32_t)coord.y & 31u,
                                     (uint32_t)coord.z & 31u};
      return coord_mask.x + (coord_mask.y << 5u) + (coord_mask.z << 10u);
    }
  }

  std::vector<mask_t> cells;
};

//----------------------------------------------------------------------------//
struct sparse_volume_t
{
  struct entry_t
  {
    uint32_t data;
    uint8_t face_flags;
  };

  inline static auto is_coord_valid(const io_ivec3_t& coord,
                                    const io_u16vec3_t& dim) -> bool
  {
    return !(coord.x >= dim.x || coord.y >= dim.y || coord.z >= dim.z ||
             coord.x < 0 || coord.y < 0 || coord.z < 0);
  }

  inline static auto is_coord_valid(const io_u8vec3_t& coord,
                                    const io_u16vec3_t& dim) -> bool
  {
    return !(coord.x >= dim.x || coord.y >= dim.y || coord.z >= dim.z);
  }

  inline static auto pack(uint8_t x, uint8_t y, uint8_t z,
                          uint8_t palette_index) -> uint32_t
  {
    return (uint32_t)x | ((uint32_t)y << 8u) | ((uint32_t)z << 16u) |
           ((uint32_t)palette_index << 24u);
  }

  inline static auto unpack(uint64_t packed_value, uint8_t& x, uint8_t& y,
                            uint8_t& z, uint8_t* palette_index = nullptr,
                            uint8_t* face_flags = nullptr)
  {
    x = packed_value & 0xFFu;
    y = (packed_value >> 8u) & 0xFFu;
    z = (packed_value >> 16u) & 0xFFu;
    if (palette_index)
      *palette_index = (packed_value >> 24u) & 0xFFu;
  }

  void mirror(io_ref_t shape, uint8_t axis_mask)
  {
    if (!axis_mask)
      return;

    const auto dim = io_component_voxel_shape->get_dim(shape);
    const auto center = io_vec3_t{dim.x * 0.5f, dim.y * 0.5f, dim.z * 0.5f};

    for (uint32_t i = 0u; i < 3u; ++i)
    {
      if ((axis_mask & (1u << i)) == 0u)
        continue;

      // Copy so we can read while creating new ones
      const auto entries_read = entries;

      for (auto e : entries_read)
      {
        io_u8vec3_t coord;
        uint8_t palette_index;
        unpack(e.data, coord.x, coord.y, coord.z, &palette_index);

        const auto coord_float =
            io_vec3_t{coord.x + 0.5f, coord.y + 0.5f, coord.z + 0.5f};

        auto coord_mirr = coord_float;
        coord_mirr.data[i] =
            center.data[i] - (coord_float.data[i] - center.data[i]);

        set(io_ivec3_t{(int16_t)coord_mirr.x, (int16_t)coord_mirr.y,
                       (int16_t)coord_mirr.z},
            palette_index, dim);
      }
    }
  }

  void remove_non_solid_voxels(io_ref_t shape)
  {
    const auto data = io_component_voxel_shape->get_voxel_data(shape);
    const auto dim = io_component_voxel_shape->get_dim(shape);

    sparse_volume_t updated_volume;

    for (auto e : entries)
    {
      io_u8vec3_t coord;
      uint8_t palette_index;
      unpack(e.data, coord.x, coord.y, coord.z, &palette_index);

      if (!is_coord_valid(coord, dim))
        continue;

      const uint32_t index =
          coord.x + coord.y * dim.x + coord.z * dim.x * dim.y;
      if (data[index] == 0u)
        continue;

      updated_volume.set(coord, palette_index, dim);
    }

    *this = updated_volume;
  }

  void apply(io_ref_t shape) const
  {
    const auto data = io_component_voxel_shape->get_voxel_data(shape);
    const auto dim = io_component_voxel_shape->get_dim(shape);

    for (auto e : entries)
    {
      io_u8vec3_t coord;
      uint8_t palette_index;
      unpack(e.data, coord.x, coord.y, coord.z, &palette_index);

      if (!is_coord_valid(coord, dim))
        continue;

      const uint32_t index =
          coord.x + coord.y * dim.x + coord.z * dim.x * dim.y;
      data[index] = palette_index;
    }

    io_component_voxel_shape->voxelize(shape);
  }

  void add_by_palette_index(io_ref_t shape, io_u8vec3_t start_coord)
  {
    const auto data = io_component_voxel_shape->get_voxel_data(shape);
    const auto dim = io_component_voxel_shape->get_dim(shape);

    const auto start_palette_index =
        data[start_coord.x + start_coord.y * dim.x +
             start_coord.z * dim.x * dim.y];

    auto read_ptr = data;
    for (uint32_t z = 0u; z < dim.z; ++z)
      for (uint32_t y = 0u; y < dim.y; ++y)
        for (uint32_t x = 0u; x < dim.x; ++x)
        {
          const uint8_t palette_index = *read_ptr++;
          if (palette_index == start_palette_index)
            set(x, y, z, palette_index, dim);
        }
  }

  void add_region(io_ref_t shape, io_u8vec3_t start_coord,
                  region_neighborhood_t neighborhood,
                  uint8_t axis_flags = 0xFFu,
                  io_ivec3_t exclude_dir = {0, 0, 0}, bool match_color = false,
                  io_ref_t palette = {}, float fuziness = 0.0f)
  {
    const auto data = io_component_voxel_shape->get_voxel_data(shape);
    const auto dim = io_component_voxel_shape->get_dim(shape);

    const auto start_palette_index =
        data[start_coord.x + start_coord.y * dim.x +
             start_coord.z * dim.x * dim.y];
    const glm::vec3 start_color = io_cvt(
        io_resource_palette->get_color(palette, start_palette_index - 1u));
    const float fuziness2 = fuziness * fuziness;

    std::vector<io_u8vec3_t> stack;
    stack.emplace_back(start_coord);
    std::vector<bool> mask(dim.x * dim.y * dim.z);

    const auto is_set = [&dim, &data, &stack, &mask, match_color,
                         start_palette_index](io_ivec3_t coord) -> bool {
      if (coord.x < 0 || coord.y < 0 || coord.z < 0 || coord.x >= dim.x ||
          coord.y >= dim.y || coord.z >= dim.z)
        return false;

      const auto index = coord.x + coord.y * dim.x + coord.z * dim.x * dim.y;
      return data[index] != 0u;
    };

    const auto add_coord = [&dim, &data, &stack, &mask, match_color,
                            start_palette_index, palette, fuziness2,
                            start_color](io_ivec3_t coord) {
      if (coord.x < 0 || coord.y < 0 || coord.z < 0 || coord.x >= dim.x ||
          coord.y >= dim.y || coord.z >= dim.z)
        return;

      const auto index = coord.x + coord.y * dim.x + coord.z * dim.x * dim.y;
      if (mask[index])
        return;

      const auto palette_index = data[index];
      bool match = palette_index != 0u;
      if (match && match_color)
      {
        if (fuziness2 > 0.0f)
        {
          const glm::vec3 color = io_cvt(
              io_resource_palette->get_color(palette, palette_index - 1u));
          match = glm::distance2(color, start_color) < fuziness2;
        }
        else
        {
          match = palette_index == start_palette_index;
        }
      }

      if (match)
      {
        stack.push_back({(uint8_t)coord.x, (uint8_t)coord.y, (uint8_t)coord.z});
        mask[index] = true;
      }
    };

    while (!stack.empty())
    {
      const auto coord = stack.back();
      stack.pop_back();

      if (exclude_dir.x != 0 || exclude_dir.y != 0 || exclude_dir.z != 0)
      {
        if (is_set({coord.x + exclude_dir.x, coord.y + exclude_dir.y,
                    coord.z + exclude_dir.z}))
          continue;
      }

      // Add this voxel to the volume
      const auto palette_index =
          data[coord.x + coord.y * dim.x + coord.z * dim.x * dim.y];
      set(coord, palette_index, dim);

      // Queue its neighbors
      if ((axis_flags & 0x01u) == 0x01u)
      {
        add_coord({coord.x + 1, coord.y, coord.z});
        add_coord({coord.x - 1, coord.y, coord.z});
      }

      if ((axis_flags & 0x02u) == 0x02u)
      {
        add_coord({coord.x, coord.y + 1, coord.z});
        add_coord({coord.x, coord.y - 1, coord.z});
      }

      if ((axis_flags & 0x04u) == 0x04u)
      {
        add_coord({coord.x, coord.y, coord.z + 1});
        add_coord({coord.x, coord.y, coord.z - 1});
      }

      if (neighborhood == region_neighborhood_18 ||
          neighborhood == region_neighborhood_26)
      {
        if ((axis_flags & 0x03u) == 0x03u)
        {
          add_coord({coord.x + 1, coord.y + 1, coord.z});
          add_coord({coord.x - 1, coord.y - 1, coord.z});
          add_coord({coord.x + 1, coord.y - 1, coord.z});
          add_coord({coord.x - 1, coord.y + 1, coord.z});
        }

        if ((axis_flags & 0x05u) == 0x05u)
        {
          add_coord({coord.x + 1, coord.y, coord.z + 1});
          add_coord({coord.x - 1, coord.y, coord.z - 1});
          add_coord({coord.x + 1, coord.y, coord.z - 1});
          add_coord({coord.x - 1, coord.y, coord.z + 1});
        }

        if ((axis_flags & 0x06u) == 0x06u)
        {
          add_coord({coord.x, coord.y + 1, coord.z + 1});
          add_coord({coord.x, coord.y - 1, coord.z - 1});
          add_coord({coord.x, coord.y + 1, coord.z - 1});
          add_coord({coord.x, coord.y - 1, coord.z + 1});
        }

        if (neighborhood == region_neighborhood_26)
        {
          if ((axis_flags & 0x07u) == 0x07u)
          {
            add_coord({coord.x + 1, coord.y + 1, coord.z + 1});
            add_coord({coord.x - 1, coord.y - 1, coord.z - 1});
            add_coord({coord.x + 1, coord.y + 1, coord.z - 1});
            add_coord({coord.x - 1, coord.y - 1, coord.z + 1});
            add_coord({coord.x + 1, coord.y - 1, coord.z + 1});
            add_coord({coord.x - 1, coord.y + 1, coord.z - 1});
            add_coord({coord.x - 1, coord.y + 1, coord.z + 1});
            add_coord({coord.x + 1, coord.y - 1, coord.z - 1});
          }
        }
      }
    };
  }

  sparse_volume_t prepare_fill(io_ref_t shape, const palette_range_t& range,
                               bool force_identical_voxels = false) const
  {
    const auto data = io_component_voxel_shape->get_voxel_data(shape);
    const auto dim = io_component_voxel_shape->get_dim(shape);

    sparse_volume_t change;
    for (auto e : entries)
    {
      io_u8vec3_t coord;
      unpack(e.data, coord.x, coord.y, coord.z);

      if (!is_coord_valid(coord, dim))
        continue;

      const uint32_t index =
          coord.x + coord.y * dim.x + coord.z * dim.x * dim.y;
      const auto palette_index = range.get_random_palette_index() + 1u;

      if (data[index] != palette_index || force_identical_voxels)
        change.set(coord, palette_index, dim);
    }

    return change;
  }

  sparse_volume_t prepare_erase(io_ref_t shape) const
  {
    const auto data = io_component_voxel_shape->get_voxel_data(shape);
    const auto dim = io_component_voxel_shape->get_dim(shape);

    sparse_volume_t change;
    for (auto e : entries)
    {
      io_u8vec3_t coord;
      unpack(e.data, coord.x, coord.y, coord.z);

      if (!is_coord_valid(coord, dim))
        continue;

      const uint32_t index =
          coord.x + coord.y * dim.x + coord.z * dim.x * dim.y;
      if (data[index] != 0u)
        change.set(coord, 0u, dim);
    }

    return change;
  }

  void update_from_shape(io_ref_t shape)
  {
    sparse_volume_t updated_volume;
    updated_volume.entries.reserve(entries.size());

    const auto data = io_component_voxel_shape->get_voxel_data(shape);
    const auto dim = io_component_voxel_shape->get_dim(shape);

    for (auto e : entries)
    {
      io_u8vec3_t coord;
      unpack(e.data, coord.x, coord.y, coord.z);

      if (!is_coord_valid(coord, dim))
        continue;

      const uint32_t index =
          coord.x + coord.y * dim.x + coord.z * dim.x * dim.y;
      const auto current_palette_index = data[index];

      updated_volume.set(coord.x, coord.y, coord.z, current_palette_index, dim);
    }

    entries = std::move(updated_volume.entries);
  }

  void draw(io_ref_t shape, bool disable_outlines = false,
            bool disable_solids = false)
  {
    const auto entity = io_component_voxel_shape->base.get_entity(shape);
    const auto palette = io_component_voxel_shape->get_palette(shape);

    if (!io_base->ref_is_valid(palette))
      return;

    const auto dim = io_component_voxel_shape->get_dim(shape);
    const auto node = io_component_node->base.get_component_for_entity(entity);

    io_debug_geometry->backface_culling_begin();

    for (auto e : entries)
    {
      io_u8vec3_t coord;
      uint8_t palette_index;
      unpack(e.data, coord.x, coord.y, coord.z, &palette_index);

      if (e.face_flags == 0u)
        continue;

      if (!is_coord_valid(coord, dim))
        continue;

      const uint32_t index =
          coord.x + coord.y * dim.x + coord.z * dim.x * dim.y;

      const auto center_vs =
          io_vec3_t{coord.x + 0.5f, coord.y + 0.5f, coord.z + 0.5f};
      const auto center_ws =
          io_component_voxel_shape->to_world_space(shape, center_vs);

      auto size = io_component_node->get_world_size(node);
      auto color = io_vec4_t{0.5f, 0.5f, 0.5f, 1.0f};
      if (palette_index > 0u)
        color = io_resource_palette->get_color(palette, palette_index - 1u);

      constexpr float inflate = 0.001f;
      if (!disable_solids)
      {
        io_debug_geometry->draw_solid_box(
            center_ws, io_component_node->get_world_orientation(node),
            {size.x + inflate, size.y + inflate, size.z + inflate}, color,
            false, e.face_flags);
      }

      if (!disable_outlines)
      {
        color.w = 0.75f;
        io_debug_geometry->draw_box(
            center_ws, io_component_node->get_world_orientation(node),
            {size.x + inflate, size.y + inflate, size.z + inflate}, color, true,
            e.face_flags);
      }
    }

    io_debug_geometry->backface_culling_end();
  }

  inline void cull_for_draw(io_ref_t shape)
  {
    const auto dim = io_component_voxel_shape->get_dim(shape);

    sparse_occupancy_mask_t mask;

    // Build mask for culling
    for (auto e : entries)
    {
      io_u8vec3_t coord;
      unpack(e.data, coord.x, coord.y, coord.z);

      if (!is_coord_valid(coord, dim))
        continue;

      mask.set(coord, true);
    }

    auto is_voxel_set = [&dim, &mask](int32_t x, int32_t y, int32_t z) -> bool {
      if (x >= dim.x || y >= dim.y || z >= dim.z || x < 0 || y < 0 || z < 0)
        return false;

      return mask.get(io_ivec3_t{x, y, z});
    };

    // Update entries according to the results
    for (auto& e : entries)
    {
      io_u8vec3_t coord;
      unpack(e.data, coord.x, coord.y, coord.z);

      const auto coord_int = io_ivec3_t{coord.x, coord.y, coord.z};

      // Check which faces are exposed
      e.face_flags = 0u;
      // Front
      e.face_flags |=
          uint8_t(!is_voxel_set(coord_int.x, coord_int.y, coord_int.z + 1))
          << io_box_face_index_front;
      // Back
      e.face_flags |=
          uint8_t(!is_voxel_set(coord_int.x, coord_int.y, coord_int.z - 1))
          << io_box_face_index_back;
      // Top
      e.face_flags |=
          uint8_t(!is_voxel_set(coord_int.x, coord_int.y + 1, coord_int.z))
          << io_box_face_index_top;
      // Bottom
      e.face_flags |=
          uint8_t(!is_voxel_set(coord_int.x, coord_int.y - 1, coord_int.z))
          << io_box_face_index_bottom;
      // Left
      e.face_flags |=
          uint8_t(!is_voxel_set(coord_int.x - 1, coord_int.y, coord_int.z))
          << io_box_face_index_left;
      // Right
      e.face_flags |=
          uint8_t(!is_voxel_set(coord_int.x + 1, coord_int.y, coord_int.z))
          << io_box_face_index_right;
    }
  }

  inline void set(int32_t x, int32_t y, int32_t z, uint8_t palette_index,
                  io_u16vec3_t max_dim)
  {
    const auto coord_int = io_ivec3_t{x, y, z};
    if (!is_coord_valid(coord_int, max_dim))
      return;

    // Skip existing entries
    const bool changed = occupancy.set(coord_int, true);
    if (!changed)
      return;

    entries.push_back({pack(x, y, z, palette_index), io_box_face_flags_all});
  }

  inline void set(io_u8vec3_t coord, uint8_t palette_index,
                  io_u16vec3_t max_dim)
  {
    set(coord.x, coord.y, coord.z, palette_index, max_dim);
  }

  inline void set(io_ivec3_t coord, uint8_t palette_index, io_u16vec3_t max_dim)
  {
    set(coord.x, coord.y, coord.z, palette_index, max_dim);
  }

  inline void add(const sparse_volume_t& other, io_u16vec3_t max_dim,
                  io_ivec3_t offset = {})
  {
    for (auto e : other.entries)
    {
      io_u8vec3_t coord;
      uint8_t palette_index;
      unpack(e.data, coord.x, coord.y, coord.z, &palette_index);

      const auto coord_int = io_ivec3_t{coord.x + offset.x, coord.y + offset.y,
                                        coord.z + offset.z};
      if (!is_coord_valid(coord_int, max_dim))
        continue;

      set(io_u8vec3_t{(uint8_t)coord_int.x, (uint8_t)coord_int.y,
                      (uint8_t)coord_int.z},
          palette_index, max_dim);
    }
  }

  inline void clear()
  {
    entries.clear();
    occupancy.clear();
  }
  inline auto empty() const -> bool { return entries.empty(); }

  std::vector<entry_t> entries;
  sparse_occupancy_mask_t<> occupancy;
};

//----------------------------------------------------------------------------//
namespace benchmark
{

//----------------------------------------------------------------------------//
static uint64_t timer_start;
static double smpl_accumulator = 0.0;
static uint32_t smpl_counter = 0u;

//----------------------------------------------------------------------------//
void timer_sample_start()
{
  timer_start = std::chrono::time_point_cast<std::chrono::nanoseconds>(
                    std::chrono::high_resolution_clock::now())
                    .time_since_epoch()
                    .count();
}

//----------------------------------------------------------------------------//
void timer_sample_end()
{
  const uint64_t timer_end =
      std::chrono::time_point_cast<std::chrono::nanoseconds>(
          std::chrono::high_resolution_clock::now())
          .time_since_epoch()
          .count();
  const uint64_t ns = timer_end - timer_start;
  smpl_accumulator += ns;
  ++smpl_counter;
}

//----------------------------------------------------------------------------//
void timer_finalize(const char* title, uint32_t result)
{
  log_info("%s (Num. Samples: %u, Result: %u): ~%.6f ms", title, smpl_counter,
           result, float(smpl_accumulator / smpl_counter * 1e-6));
  smpl_accumulator = 0.0;
  smpl_counter = 0u;
}

//----------------------------------------------------------------------------//
template <bool UseMorton, typename T>
static auto occupancy_mask_test0(uint32_t size, T& mask) -> uint32_t
{
  uint32_t result = 0u;

  const uint32_t num_voxels = size * size * size;

  for (uint32_t i = 0u; i < num_voxels; i += 8u)
  {
    const auto c = morton::decode_8b(i % num_voxels);

    result += mask.get(io_uvec3_t{c.x, c.y, c.z});
    result += mask.get(io_uvec3_t{c.x + 1u, c.y, c.z});
    result += mask.get(io_uvec3_t{c.x, c.y + 1u, c.z});
    result += mask.get(io_uvec3_t{c.x, c.y, c.z + 1u});
    result += mask.get(io_uvec3_t{c.x + 1u, c.y + 1u, c.z});
    result += mask.get(io_uvec3_t{c.x + 1u, c.y, c.z + 1u});
    result += mask.get(io_uvec3_t{c.x, c.y + 1u, c.z + 1u});
    result += mask.get(io_uvec3_t{c.x + 1u, c.y + 1u, c.z + 1u});
  }

  return result;
}

//----------------------------------------------------------------------------//
void occupancy_mask()
{
  sparse_occupancy_mask_t<true> mask_morton;
  sparse_occupancy_mask_t<false> mask_linear;

  constexpr uint32_t size = 64u;
  constexpr uint32_t num_samples = 10u;

  for (uint32_t z = 0u; z < size; ++z)
    for (uint32_t y = 0u; y < size; ++y)
      for (uint32_t x = 0u; x < size; ++x)
      {
        mask_morton.set(io_uvec3_t{x, y, z}, true);
        mask_linear.set(io_uvec3_t{x, y, z}, true);
      }

  uint32_t result;
  for (uint32_t i = 0u; i < num_samples; ++i)
  {
    timer_sample_start();
    result = occupancy_mask_test0<true>(size, mask_morton);
    timer_sample_end();
  }
  timer_finalize("Morton", result);

  for (uint32_t i = 0u; i < num_samples; ++i)
  {
    timer_sample_start();
    result = occupancy_mask_test0<false>(size, mask_linear);
    timer_sample_end();
  }
  timer_finalize("Linear", result);
}
} // namespace benchmark