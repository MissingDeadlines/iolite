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

#define EDITING_ENABLE_LOGGING 0

// Dependencies
#include "glm.hpp"
#include "gtc/quaternion.hpp"
#include "gtx/norm.hpp"
#include "imgui.h"
#include "IconsFontAwesome6.h"

#if EDITING_ENABLE_LOGGING > 0
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"
#endif

// STL
#include <cstring>
#include <vector>
#include <immintrin.h>

// API
#define IO_USER_U16VEC3_TYPE glm::u16vec3
#define IO_USER_IVEC3_TYPE glm::ivec3
#define IO_USER_VEC3_TYPE glm::vec3
#define IO_USER_VEC4_TYPE glm::vec4
#include "iolite_api.h"

// Interfaces we use
//----------------------------------------------------------------------------//
static const io_api_manager_i* io_api_manager = nullptr;
static const io_base_i* io_base = nullptr;
static const io_logging_i* io_logging = nullptr;
static const io_component_node_i* io_component_node = nullptr;
static const io_component_voxel_shape_i* io_component_voxel_shape = nullptr;
static const io_resource_palette_i* io_resource_palette = nullptr;
static const io_editor_i* io_editor = nullptr;
static const io_world_i* io_world = nullptr;
static const io_debug_geometry_i* io_debug_geometry = nullptr;
static const io_input_system_i* io_input_system = nullptr;
static const io_low_level_imgui_i* io_low_level_imgui = nullptr;

// Interfaces we provide
//----------------------------------------------------------------------------//
static io_user_editor_tool_i io_user_editor_tool = {};

//----------------------------------------------------------------------------//
static uint64_t rng_seed = 42ull;

//----------------------------------------------------------------------------//
enum region_neighborhood_
{
  region_neighborhood_6,
  region_neighborhood_18,
  region_neighborhood_26,
};
using region_neighborhood_t = int32_t;

// https://fgiesen.wordpress.com/2009/12/13/decoding-morton-codes/
// https://fgiesen.wordpress.com/2022/09/09/morton-codes-addendum/
namespace morton
{
inline uint32_t part_1by2_5b(uint32_t x)
{
  return _pdep_u32(x & 0x1Fu, 0x1249u);
}

inline uint32_t part_1by2_8b(uint32_t x)
{
  return _pdep_u32(x & 0xFFu, 0x249249u);
}

inline uint32_t comp_1by2_8b(uint32_t x)
{
  return _pext_u32(x, 0x249249u) & 0xFFu;
}

#if 0
template <typename T> inline uint32_t encode_5b(const T& v)
{
  return (part_1by2_5b(v.z) << 2) + (part_1by2_5b(v.y) << 1) +
         part_1by2_5b(v.x);
}
#else
template <typename T> inline uint32_t encode_5b(const T& v)
{
  const uint64_t x =
      (v.x & 0x1Fu) | ((v.y & 0x1Fu) << 5u) | ((v.z & 0x1Fu) << 10u);
  const uint64_t y = _pdep_u64(
      x, 0x49249249249ull); // 001001001001001001001001001001001001001001001
  constexpr uint64_t mask = 0x1249u; // 001001001001001
  return (y & mask) | ((y >> 14u) & (mask << 1u)) | ((y >> 28u) & (mask << 2u));
}
#endif

template <typename T> inline uint32_t encode_8b(const T& v)
{
  return (part_1by2_8b(v.z) << 2) + (part_1by2_8b(v.y) << 1) +
         part_1by2_8b(v.x);
}

inline io_u8vec3_t decode_8b(uint32_t c)
{
  return {(io_uint8_t)comp_1by2_8b(c), (io_uint8_t)comp_1by2_8b(c >> 1u),
          (io_uint8_t)comp_1by2_8b(c >> 2u)};
}
} // namespace morton

namespace common
{
//----------------------------------------------------------------------------//
inline auto hash(const char* data, uint32_t size) -> uint32_t
{
  if (nullptr == data || size == 0u)
    return 0u;

  uint32_t hash = 5381u;
  for (uint32_t i = 0u; i < size; ++i)
  {
    hash = ((hash << 5) + hash) + uint8_t(data[i]);
  }

  return hash;
}

//----------------------------------------------------------------------------//
inline static void srand(io_u8vec3_t coord)
{
  rng_seed = coord.x | ((uint64_t)coord.y << 8u) | ((uint64_t)coord.z << 16u);
}

//----------------------------------------------------------------------------//
inline static auto rand(uint64_t& seed = rng_seed) -> uint64_t
{
  seed ^= seed << 13;
  seed ^= seed >> 7;
  seed ^= seed << 17;
  return seed;
}

//----------------------------------------------------------------------------//
inline static auto rand_float(float min, float max, uint64_t& seed = rng_seed)
    -> float
{
  return uint32_t(rand(seed)) /
             static_cast<float>(std::numeric_limits<uint32_t>::max()) *
             (max - min) +
         min;
}
} // namespace common

//----------------------------------------------------------------------------//
struct palette_range_t
{
  inline palette_range_t() { palette_indices.emplace_back(0u); }
  inline palette_range_t(io_uint8_t palette_index)
  {
    palette_indices.emplace_back(palette_index);
  }
  inline palette_range_t(io_uint8_t palette_index_start,
                         io_uint8_t palette_index_end)
  {
    for (io_uint8_t idx = palette_index_start; idx <= palette_index_end; ++idx)
      palette_indices.emplace_back(idx);
  }

  inline auto add_or_remove_palette_index(io_uint8_t palette_index)
  {
    // Remove if the index exists
    for (auto it = palette_indices.begin(); it != palette_indices.end(); ++it)
    {
      if (*it == palette_index)
      {
        // Always keep at least one index
        if (palette_indices.size() > 1u)
          palette_indices.erase(it);

        return;
      }
    }

    // Insert otherwise
    palette_indices.emplace_back(palette_index);
  }

  inline auto has_palette_index(uint8_t palette_index) const -> bool
  {
    for (auto idx : palette_indices)
    {
      if (idx == palette_index)
        return true;
    }

    return false;
  }

  inline auto get_first_palette_index() const -> io_uint8_t
  {
    return palette_indices.front();
  }

  inline auto get_random_palette_index() const -> io_uint8_t
  {
    return palette_indices[common::rand() % palette_indices.size()];
  }

  std::vector<io_uint8_t> palette_indices;
};

//----------------------------------------------------------------------------//
#if EDITING_ENABLE_LOGGING > 0
//----------------------------------------------------------------------------//
inline static auto log_info(const char* fmt, ...)
{
  char buffer[256];

  va_list args;
  va_start(args, fmt);
  stbsp_vsnprintf(buffer, 256, fmt, args);
  va_end(args);

  io_logging->log_plugin("Voxel Editing", buffer);
}
//----------------------------------------------------------------------------//
#else
//----------------------------------------------------------------------------//
inline static auto log_info(const char* fmt, ...) {}
//----------------------------------------------------------------------------//
#endif
//----------------------------------------------------------------------------//