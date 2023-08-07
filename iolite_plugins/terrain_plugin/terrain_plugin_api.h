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

#include "iolite_api.h"

#ifndef INCLUDE_IO_PLUGIN_TERRAIN_API
#define INCLUDE_IO_PLUGIN_TERRAIN_API

// A single pixel of a heightmap
//----------------------------------------------------------------------------//
typedef struct
{
  io_uint32_t internal;
} io_plugin_terrain_heightmap_pixel;

//----------------------------------------------------------------------------//
inline io_plugin_terrain_heightmap_pixel
io_plugin_terrain_create_heightmap_pixel(io_float32_t height,
                                         io_float32_t grass_height,
                                         io_uint8_t palette_index)
{
  const io_uint32_t v_height = io_min(height * 255.0f, 255u);
  const io_uint32_t v_grass_height = io_min(grass_height * 255.0f, 255u);
  const io_uint32_t v_palette_index = palette_index;

  io_plugin_terrain_heightmap_pixel p;
  p.internal = v_height | (v_grass_height << 8u) | (palette_index << 16u);

  return p;
}

//----------------------------------------------------------------------------//
#define IO_PLUGIN_TERRAIN_API_NAME "io_plugin_terrain_i"
//----------------------------------------------------------------------------//

// Inteface exposed by the terrain (generator) plugin
//----------------------------------------------------------------------------//
struct io_plugin_terrain_i
{
  // Generates heightmap based terrain from the provided data.
  io_ref_t (*generate_from_data)(
      const io_plugin_terrain_heightmap_pixel* heightmap, io_uint32_t size,
      const char* palette_name, io_float32_t max_height,
      io_float32_t voxel_size);
  // Generate heightmap based terrain from the provided image.
  io_ref_t (*generate_from_image)(const char* heightmap_filename,
                                  const char* palette_name,
                                  io_float32_t max_height,
                                  io_float32_t voxel_size);
};

#endif