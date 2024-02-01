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

// Dependencies
#include "glm.hpp"
#include "gtc/packing.hpp"
#include <vector>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// API
#define IO_USER_VEC2_TYPE glm::vec2
#define IO_USER_VEC3_TYPE glm::vec3
#define IO_USER_VEC4_TYPE glm::vec4
#define IO_USER_QUAT_TYPE glm::quat
#define IO_USER_UVEC2_TYPE glm::uvec2
#define IO_USER_UVEC3_TYPE glm::uvec3
#define IO_USER_UVEC4_TYPE glm::uvec4
#define IO_USER_IVEC2_TYPE glm::ivec2
#define IO_USER_IVEC3_TYPE glm::ivec3
#define IO_USER_IVEC4_TYPE glm::ivec4
#include "iolite_api.h"
#include "terrain_plugin_api.h"

// Settings
//----------------------------------------------------------------------------//
#define CHUNK_SIZE 32u

// Interfaces we use
//----------------------------------------------------------------------------//
const io_api_manager_i* io_api_manager = nullptr;
const io_base_i* io_base = nullptr;
const io_logging_i* io_logging = nullptr;
const io_filesystem_i* io_filesystem = nullptr;
const io_component_node_i* io_component_node = nullptr;
const io_component_voxel_shape_i* io_component_voxel_shape = nullptr;

// Interfaces we provide
//----------------------------------------------------------------------------//
static io_plugin_terrain_i io_plugin_terrain = {};

//----------------------------------------------------------------------------//
struct chunk_collection_t
{
  std::vector<io_ref_t> chunks;
};

//----------------------------------------------------------------------------//
inline auto get_data_for_chunk_pos(const glm::uvec2 chunk_pos,
                                   const glm::uvec2 chunk_idx,
                                   io_uint32_t heightmap_pitch,
                                   io_uint32_t* const heightmap) -> glm::u8vec4
{
  const auto pixel_pos =
      glm::uvec2((CHUNK_SIZE - 1u - chunk_pos.x) + chunk_idx.x * CHUNK_SIZE,
                 chunk_pos.y + chunk_idx.y * CHUNK_SIZE);
  const auto pixel_idx = pixel_pos.y * heightmap_pitch + pixel_pos.x;

  return glm::unpackUint4x8(heightmap[pixel_idx]);
}

//----------------------------------------------------------------------------//
auto calc_min_max_height_for_chunk(const glm::uvec2 chunk_idx,
                                   uint32_t heightmap_pitch,
                                   uint32_t* const heightmap, float max_height,
                                   uint32_t num_chunks_xz) -> glm::uvec2
{
  uint32_t max_h = 0u;
  uint32_t min_h = UINT_MAX;

  for (uint32_t chunk_pos_z = 0u; chunk_pos_z < CHUNK_SIZE; ++chunk_pos_z)
    for (uint32_t chunk_pos_x = 0u; chunk_pos_x < CHUNK_SIZE; ++chunk_pos_x)
    {
      const auto data =
          get_data_for_chunk_pos(glm::uvec2(chunk_pos_x, chunk_pos_z),
                                 chunk_idx, heightmap_pitch, heightmap);
      auto h = uint32_t((data.x / 255.0f) * max_height);
      const uint32_t gh = data.g;
      h += gh;

      max_h = glm::max(max_h, h);
      min_h = glm::min(min_h, h);
    }

  return glm::uvec2(min_h, max_h);
}

//----------------------------------------------------------------------------//
auto calc_min_max_height_for_adjacent_chunks(const glm::uvec2 chunk_idx,
                                             uint32_t heightmap_pitch,
                                             uint32_t* const heightmap,
                                             float max_height,
                                             uint32_t num_chunks_xz)
    -> glm::uvec2
{
  const auto ci0 = chunk_idx + glm::uvec2(1u, 0u);
  const auto ci1 = chunk_idx - glm::uvec2(1u, 0u);
  const auto ci2 = chunk_idx + glm::uvec2(0u, 1u);
  const auto ci3 = chunk_idx - glm::uvec2(0u, 1u);

  const glm::uvec2 cis[] = {ci0, ci1, ci2, ci3};

  auto min_max = glm::uvec2(UINT_MAX, 0u);

  for (auto ci : cis)
  {
    if (glm::all(glm::lessThan(ci, glm::uvec2(num_chunks_xz))))
    {
      const auto mm = calc_min_max_height_for_chunk(
          ci, heightmap_pitch, heightmap, max_height, num_chunks_xz);
      min_max.x = glm::min(mm.x, min_max.x);
      min_max.y = glm::min(mm.y, min_max.y);
    }
  }

  return min_max;
}

//----------------------------------------------------------------------------//
struct terrain_task_t : public io_scheduler_task_t
{
  static void execute(io_uvec2_t range, uint32_t thread_id,
                      uint32_t sub_task_index, void* task_)
  {
    auto task = (terrain_task_t*)task_;

    for (uint32_t i = range.x; i < range.y; ++i)
    {
      const glm::uvec2 chunk_idx =
          glm::uvec2(i % task->num_chunks, i / task->num_chunks);
      const auto& ce = (*task->chunks)[chunk_idx.x][chunk_idx.y];

      for (uint32_t chunk_pos_z = 0u; chunk_pos_z < CHUNK_SIZE; ++chunk_pos_z)
        for (uint32_t chunk_pos_x = 0u; chunk_pos_x < CHUNK_SIZE; ++chunk_pos_x)
        {
          const auto data = get_data_for_chunk_pos(
              glm::uvec2(chunk_pos_x, chunk_pos_z), chunk_idx,
              task->heightmap_pitch, task->heightmap);
          const auto h = uint32_t(data.r / 255.0f * task->max_height);
          const auto gh = data.g;
          const auto mat = data.b + 1u;

          // The number of vertical chunks we have to modify
          const auto num_chunks = (h + gh) / CHUNK_SIZE + 1u;

          uint32_t mat_voxels_to_set = h;
          uint32_t grass_voxels_to_set = gh;

          for (uint32_t i = 0u; i < num_chunks; ++i)
          {
            auto c = ce.chunks[i];

            uint32_t write_start = 0u;

            // Add material
            if (mat_voxels_to_set > 0u)
            {
              const uint32_t voxels_to_set =
                  glm::min(mat_voxels_to_set, CHUNK_SIZE);

              if (io_base->ref_is_valid(c))
              {
                auto vox_data = io_component_voxel_shape->get_voxel_data(c);
                memset(&vox_data[chunk_pos_x * CHUNK_SIZE +
                                 chunk_pos_z * CHUNK_SIZE * CHUNK_SIZE],
                       mat, voxels_to_set);
              }

              mat_voxels_to_set -= voxels_to_set;
              write_start = voxels_to_set;
            }

            // Add grass
            if (mat_voxels_to_set == 0u && grass_voxels_to_set > 0u)
            {
              const uint32_t voxels_to_set =
                  glm::min(grass_voxels_to_set, CHUNK_SIZE - write_start);

              if (io_base->ref_is_valid(c))
              {
                auto voxel_data = io_component_voxel_shape->get_voxel_data(c);
                memset(&voxel_data[chunk_pos_x * CHUNK_SIZE +
                                   chunk_pos_z * CHUNK_SIZE * CHUNK_SIZE] +
                           write_start,
                       mat, voxels_to_set);
              }
              grass_voxels_to_set -= voxels_to_set;
            }
          }
        }
    }
  }

  std::vector<std::vector<chunk_collection_t>>* chunks;
  uint32_t num_chunks;
  uint32_t heightmap_pitch;
  uint32_t* heightmap;
  float max_height;
} terrain_task_set;

//----------------------------------------------------------------------------//
auto generate_from_data(const io_plugin_terrain_heightmap_pixel* heightmap,
                        const io_uint32_t size, const char* palette_name,
                        io_float32_t max_height, io_float32_t voxel_size)
    -> io_ref_t
{
  if (size % CHUNK_SIZE != 0u)
  {
    io_logging->log_warning(
        "Terrain size needs to be a multiple of the chunk size of 32 voxels.");
    return io_base->ref_invalid();
  }

  const uint32_t num_voxels = size;
  const uint32_t num_chunks_xz = num_voxels / CHUNK_SIZE;

  auto terrain_node = io_component_node->create("terrain");
  io_component_node->update_transforms(terrain_node);

  std::vector<std::vector<chunk_collection_t>> chunks;
  chunks.resize(num_chunks_xz);
  for (auto& cs : chunks)
    cs.resize(num_chunks_xz);

  constexpr bool skip_enclosed_chunks = true;

  for (uint32_t z = 0u; z < num_chunks_xz; ++z)
    for (uint32_t x = 0u; x < num_chunks_xz; ++x)
    {
      const auto min_max_height = calc_min_max_height_for_chunk(
          glm::uvec2(x, z), size, (uint32_t*)heightmap, max_height,
          num_chunks_xz);

      const uint32_t num_vertical_chunks_max =
          min_max_height.y / CHUNK_SIZE + 1u;
      const uint32_t num_vertical_chunks_min =
          min_max_height.x / CHUNK_SIZE + 1u;

      std::vector<io_ref_t> vertical_chunks;
      vertical_chunks.reserve(num_vertical_chunks_max);

      for (uint32_t y = 0u; y < num_vertical_chunks_max; ++y)
      {
        // Skip empty chunks
        if (min_max_height.y == 0u)
        {
          vertical_chunks.emplace_back(io_base->ref_invalid());
          continue;
        }

        // Skip enclosed chunks
        if (skip_enclosed_chunks && num_vertical_chunks_min >= 2u &&
            y < num_vertical_chunks_min - 2u)
        {
          vertical_chunks.emplace_back(io_base->ref_invalid());
          continue;
        }

        // Set up entity for this chunk
        {
          auto chunk_node = io_component_node->create_with_parent(
              "terrain_chunk", terrain_node);
          auto chunk_entity = io_component_node->base.get_entity(chunk_node);

          io_component_node->set_position(
              chunk_node,
              io_cvt(glm::vec3(x, y, z) * (float)CHUNK_SIZE * voxel_size));
          io_component_node->set_orientation(
              chunk_node,
              io_cvt(glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(90.0f)))));
          io_component_node->set_size(chunk_node,
                                      {voxel_size, voxel_size, voxel_size});
          io_component_node->update_transforms(chunk_node);

          auto shape = io_component_voxel_shape->base.create(chunk_entity);
          io_component_voxel_shape->base.set_property(
              shape, "PaletteName", io_variant_from_string(palette_name));
          io_component_voxel_shape->base.set_property(
              shape, "CustomSize",
              io_variant_from_u16vec3({CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE}));
          io_component_voxel_shape->base.commit_changes(shape);

          vertical_chunks.emplace_back(shape);
        }
      }

      chunks[x][z] = {std::move(vertical_chunks)};
    }

  // Set up and dispatch tasks
  {
    io_init_scheduler_task(&terrain_task_set, num_chunks_xz * num_chunks_xz,
                           terrain_task_t::execute);
    terrain_task_set.chunks = &chunks;
    terrain_task_set.num_chunks = num_chunks_xz;
    terrain_task_set.heightmap_pitch = size;
    terrain_task_set.max_height = max_height;
    terrain_task_set.heightmap = (uint32_t*)heightmap;

    io_base->scheduler_enqueue_task(&terrain_task_set);
    io_base->scheduler_wait_for_task(&terrain_task_set);
  }

  for (auto& cx : chunks)
    for (auto& cz : cx)
      for (auto cy : cz.chunks)
      {
        if (io_base->ref_is_valid(cy))
          io_component_voxel_shape->voxelize(cy);
      }

  return terrain_node;
}

//----------------------------------------------------------------------------//
auto generate_from_image(const char* heightmap_name, const char* palette_name,
                         io_float32_t max_height, io_float32_t voxel_size)
    -> io_ref_t
{
  const std::string filepath =
      std::string("/media/heightmaps/") + heightmap_name;

  io_uint32_t length;
  io_filesystem->load_file_from_data_source(filepath.c_str(), nullptr, &length);

  std::vector<uint8_t> data(length);
  if (!io_filesystem->load_file_from_data_source(filepath.c_str(), data.data(),
                                                 &length))
  {
    return io_base->ref_invalid();
  }

  int32_t width, height;
  auto* heightmap = (uint32_t*)stbi_load_from_memory(
      data.data(), (int32_t)data.size(), &width, &height, nullptr, 4);

  // Validate heightmap
  if (width != height)
  {
    io_logging->log_warning("Terrain needs to be square.");

    // Clean up
    free(heightmap);
    return io_base->ref_invalid();
  }

  io_ref_t result =
      generate_from_data((io_plugin_terrain_heightmap_pixel*)heightmap, width,
                         palette_name, max_height, voxel_size);

  // Clean up
  free(heightmap);

  return result;
}

//----------------------------------------------------------------------------//
IO_API_EXPORT io_uint32_t IO_API_CALL get_api_version()
{
  return IO_API_VERSION;
}

//----------------------------------------------------------------------------//
IO_API_EXPORT int IO_API_CALL load_plugin(void* api_manager)
{
  io_api_manager = (const io_api_manager_i*)api_manager;

  // Retrieve the interfaces we use
  {
    io_base = (const io_base_i*)io_api_manager->find_first(IO_BASE_API_NAME);
    io_logging =
        (const io_logging_i*)io_api_manager->find_first(IO_LOGGING_API_NAME);
    io_filesystem = (const io_filesystem_i*)io_api_manager->find_first(
        IO_FILESYSTEM_API_NAME);
    io_component_node = (const io_component_node_i*)io_api_manager->find_first(
        IO_COMPONENT_NODE_API_NAME);
    io_component_voxel_shape =
        (const io_component_voxel_shape_i*)io_api_manager->find_first(
            IO_COMPONENT_VOXEL_SHAPE_API_NAME);
  }

  // Register the interfaces we provide
  {
    io_plugin_terrain.generate_from_data = generate_from_data;
    io_plugin_terrain.generate_from_image = generate_from_image;

    io_api_manager->register_api(IO_PLUGIN_TERRAIN_API_NAME,
                                 &io_plugin_terrain);
  }

  return 0;
}

//----------------------------------------------------------------------------//
IO_API_EXPORT void IO_API_CALL unload_plugin()
{
  io_api_manager->unregister_api(&io_plugin_terrain);
}
