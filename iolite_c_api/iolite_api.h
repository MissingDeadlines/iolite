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

//----------------------------------------------------------------------------//
// Minimal C/C++ plugin sample
//----------------------------------------------------------------------------//

/*
#include "iolite_api.h"

const struct io_api_manager_i* io_api_manager = 0;

IO_API_EXPORT io_uint32_t IO_API_CALL get_api_version()
{
  // Inform IOLITE which version of the API you are using
  return IO_API_VERSION;
}

IO_API_EXPORT io_int32_t IO_API_CALL load_plugin(const void* api_manager)
{
  // Ensure we can keep accessing the API manager after loading the plugin
  io_api_manager = (const struct io_api_manager_i*)api_manager;

  // Do something with the API manager, set up your plugin, etc.

  return 0; // Return a value < 0 to indicate that the loading of your plugin
            // has failed (depedency not available, etc.)
}

IO_API_EXPORT void IO_API_CALL unload_plugin()
{
  // Clean up here
}
*/

#ifndef INCLUDE_IO_API
#define INCLUDE_IO_API

//----------------------------------------------------------------------------//
// IOLITE API version
//----------------------------------------------------------------------------//

// The major, minor, and bugfix parts of the IOLITE API version
//----------------------------------------------------------------------------//
#define IO_API_VERSION_MAJOR 0
#define IO_API_VERSION_MINOR 2
#define IO_API_VERSION_BUGFIX 2

// The version of the IOLITE API as a single number
//----------------------------------------------------------------------------//
#define IO_API_VERSION                                                         \
  ((IO_API_VERSION_MAJOR << 24) + (IO_API_VERSION_MINOR << 16) +               \
   (IO_API_VERSION_BUGFIX << 8) + 0)

// Helpers for retrieving the major, minor, and bugfix part from the version
// number
//----------------------------------------------------------------------------//
#define IO_API_VERSION_GET_MAJOR(_version) ((_version >> 24) & 0xFFu)
#define IO_API_VERSION_GET_MINOR(_version) ((_version >> 16) & 0xFFu)
#define IO_API_VERSION_GET_BUGFIX(_version) ((_version >> 8) & 0xFFu)

//----------------------------------------------------------------------------//
// DLL export helpers
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#ifdef _WIN32
//----------------------------------------------------------------------------//
#define IO_API_CALL __stdcall
//----------------------------------------------------------------------------//
#ifdef __cplusplus
//----------------------------------------------------------------------------//
#define IO_API_EXPORT extern "C" __declspec(dllexport)
//----------------------------------------------------------------------------//
#else // __cplusplus
//----------------------------------------------------------------------------//
#define IO_API_EXPORT __declspec(dllexport)
//----------------------------------------------------------------------------//
#endif // __cplusplus
//----------------------------------------------------------------------------//
#else // _WIN32
//----------------------------------------------------------------------------//
#ifdef __cplusplus
//----------------------------------------------------------------------------//
#define IO_API_EXPORT extern "C"
//----------------------------------------------------------------------------//
#else // __cplusplus
//----------------------------------------------------------------------------//
#define IO_API_EXPORT
//----------------------------------------------------------------------------//
#endif
//----------------------------------------------------------------------------//
#define IO_API_CALL
//----------------------------------------------------------------------------//
#endif // _WIN32
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Basic types
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#define IO_TRUE 1
#define IO_FALSE 0
//----------------------------------------------------------------------------//
#ifndef __cplusplus
//----------------------------------------------------------------------------//
typedef unsigned char io_bool_t;
#else
typedef bool io_bool_t;
//----------------------------------------------------------------------------//
#endif
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
typedef unsigned char io_uint8_t;
typedef char io_int8_t;

//----------------------------------------------------------------------------//
typedef unsigned short io_uint16_t;
typedef short io_int16_t;

//----------------------------------------------------------------------------//
typedef unsigned int io_uint32_t;
typedef int io_int32_t;

//----------------------------------------------------------------------------//
typedef unsigned long long io_uint64_t;
typedef long long io_int64_t;

//----------------------------------------------------------------------------//
typedef float io_float32_t;
typedef double io_float64_t;

//----------------------------------------------------------------------------//
// Math related types
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
typedef struct
{
  io_float32_t x, y;
} io_vec2_t;

//----------------------------------------------------------------------------//
typedef struct
{
  io_float32_t x, y, z;
} io_vec3_t;

//----------------------------------------------------------------------------//
typedef struct
{
  io_float32_t x, y, z, w;
} io_vec4_t;

//----------------------------------------------------------------------------//
typedef struct
{
  io_float32_t w, x, y, z;
} io_quat_t;

//----------------------------------------------------------------------------//
typedef struct
{
  io_int32_t x, y;
} io_ivec2_t;

//----------------------------------------------------------------------------//
typedef struct
{
  io_int32_t x, y, z;
} io_ivec3_t;

//----------------------------------------------------------------------------//
typedef struct
{
  io_int32_t x, y, z, w;
} io_ivec4_t;

//----------------------------------------------------------------------------//
typedef struct
{
  io_uint32_t x, y;
} io_uvec2_t;

//----------------------------------------------------------------------------//
typedef struct
{
  io_uint32_t x, y, z;
} io_uvec3_t;

//----------------------------------------------------------------------------//
typedef struct
{
  io_uint8_t x, y, z;
} io_u8vec3_t;

//----------------------------------------------------------------------------//
typedef struct
{
  io_uint16_t x, y, z;
} io_u16vec3_t;

//----------------------------------------------------------------------------//
typedef struct
{
  io_uint32_t x, y, z, w;
} io_uvec4_t;

//----------------------------------------------------------------------------//
// Math type conversion helpers
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#ifdef IO_USER_VEC2_TYPE
//----------------------------------------------------------------------------//
inline io_vec2_t io_cvt(IO_USER_VEC2_TYPE v) { return {v.x, v.y}; }
inline IO_USER_VEC2_TYPE io_cvt(io_vec2_t v) { return {v.x, v.y}; }
//----------------------------------------------------------------------------//
#endif // IO_USER_VEC2_TYPE
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#ifdef IO_USER_VEC3_TYPE
//----------------------------------------------------------------------------//
inline io_vec3_t io_cvt(IO_USER_VEC3_TYPE v) { return {v.x, v.y, v.z}; }
inline IO_USER_VEC3_TYPE io_cvt(io_vec3_t v) { return {v.x, v.y, v.z}; }
//----------------------------------------------------------------------------//
#endif // IO_USER_VEC3_TYPE
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#ifdef IO_USER_VEC4_TYPE
//----------------------------------------------------------------------------//
inline io_vec4_t io_cvt(IO_USER_VEC4_TYPE v) { return {v.x, v.y, v.z, v.w}; }
inline IO_USER_VEC4_TYPE io_cvt(io_vec4_t v) { return {v.x, v.y, v.z, v.w}; }
//----------------------------------------------------------------------------//
#endif // IO_USER_VEC4_TYPE
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#ifdef IO_USER_QUAT_TYPE
//----------------------------------------------------------------------------//
inline io_quat_t io_cvt(IO_USER_QUAT_TYPE v) { return {v.w, v.x, v.y, v.z}; }
//----------------------------------------------------------------------------//
#ifndef IO_USER_QUAT_TYPE_W_LAST
//----------------------------------------------------------------------------//
inline IO_USER_QUAT_TYPE io_cvt(io_quat_t v) { return {v.w, v.x, v.y, v.z}; }
//----------------------------------------------------------------------------//
#else
//----------------------------------------------------------------------------//
inline IO_USER_QUAT_TYPE io_cvt(io_quat_t v) { return {v.x, v.y, v.z, v.w}; }
//----------------------------------------------------------------------------//
#endif // IO_USER_QUAT_TYPE_W_LAST
//----------------------------------------------------------------------------//
#endif // IO_USER_QUAT_TYPE
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#ifdef IO_USER_UVEC2_TYPE
//----------------------------------------------------------------------------//
inline io_uvec2_t io_cvt(IO_USER_UVEC2_TYPE v) { return {v.x, v.y}; }
inline IO_USER_UVEC2_TYPE io_cvt(io_uvec2_t v) { return {v.x, v.y}; }
//----------------------------------------------------------------------------//
#endif // IO_USER_UVEC2_TYPE
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#ifdef IO_USER_UVEC3_TYPE
//----------------------------------------------------------------------------//
inline io_uvec3_t io_cvt(IO_USER_UVEC3_TYPE v) { return {v.x, v.y, v.z}; }
inline IO_USER_UVEC3_TYPE io_cvt(io_uvec3_t v) { return {v.x, v.y, v.z}; }
//----------------------------------------------------------------------------//
#endif // IO_USER_UVEC3_TYPE
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#ifdef IO_USER_UVEC4_TYPE
//----------------------------------------------------------------------------//
inline io_uvec4_t io_cvt(IO_USER_UVEC4_TYPE v) { return {v.x, v.y, v.z, v.w}; }
inline IO_USER_UVEC4_TYPE io_cvt(io_uvec4_t v) { return {v.x, v.y, v.z, v.w}; }
//----------------------------------------------------------------------------//
#endif // IO_USER_UVEC4_TYPE
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#ifdef IO_USER_IVEC2_TYPE
//----------------------------------------------------------------------------//
inline io_ivec2_t io_cvt(IO_USER_IVEC2_TYPE v) { return {v.x, v.y}; }
inline IO_USER_IVEC2_TYPE io_cvt(io_ivec2_t v) { return {v.x, v.y}; }
//----------------------------------------------------------------------------//
#endif // IO_USER_IVEC2_TYPE
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#ifdef IO_USER_IVEC3_TYPE
//----------------------------------------------------------------------------//
inline io_ivec3_t io_cvt(IO_USER_IVEC3_TYPE v) { return {v.x, v.y, v.z}; }
inline IO_USER_IVEC3_TYPE io_cvt(io_ivec3_t v) { return {v.x, v.y, v.z}; }
//----------------------------------------------------------------------------//
#endif // IO_USER_IVEC3_TYPE
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#ifdef IO_USER_IVEC4_TYPE
//----------------------------------------------------------------------------//
inline io_ivec4_t io_cvt(IO_USER_IVEC4_TYPE v) { return {v.x, v.y, v.z, v.w}; }
inline IO_USER_IVEC4_TYPE io_cvt(io_ivec4_t v) { return {v.x, v.y, v.z, v.w}; }
//----------------------------------------------------------------------------//
#endif // IO_USER_IVEC4_TYPE
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Loosely typed enums and flags
//----------------------------------------------------------------------------//

// Flags for configuring properties for custom components
enum io_property_flags_
{
  io_property_flags_runtime_only =
      0x01u // Indicates that the given property should neither be serialized
            // nor exposed in the editor
};
typedef io_uint8_t io_property_flags;

// Vertical text alignment
//----------------------------------------------------------------------------//
enum io_ui_text_align_vertical_
{
  io_ui_text_align_vertical_top,    // Position text at top
  io_ui_text_align_vertical_center, // Position text at vertical center
  io_ui_text_align_vertical_bottom  // Position text at bottom
};
typedef io_uint32_t io_ui_text_align_vertical;

// Horizontal text alignment
//----------------------------------------------------------------------------//
enum io_ui_text_align_horizontal_
{
  io_ui_text_align_horizontal_left,   // Position text left
  io_ui_text_align_horizontal_center, // Position text at horizontal center
  io_ui_text_align_horizontal_right   // Position text right
};
typedef io_uint32_t io_ui_text_align_horizontal;

// Various presets for anchors
//----------------------------------------------------------------------------//
enum io_ui_anchor_preset_
{
  io_ui_anchor_preset_full_rect,     // A full rectangle
  io_ui_anchor_preset_top_left,      // Anchor at top left
  io_ui_anchor_preset_top_right,     // Anchor at top right
  io_ui_anchor_preset_bottom_right,  // Anchor at bottom right
  io_ui_anchor_preset_bottom_left,   // Anchor at bottom left
  io_ui_anchor_preset_center_left,   // Anchor at the left center
  io_ui_anchor_preset_center_top,    // Anchor at the top center
  io_ui_anchor_preset_center_right,  // Anchor at the right center
  io_ui_anchor_preset_center_bottom, // Anchor at the bottom center
  io_ui_anchor_preset_center,        // Anchor at the center

  io_ui_anchor_preset_num
};
typedef io_uint32_t io_ui_anchor_preset;

//----------------------------------------------------------------------------//
enum io_ui_aspect_mode_
{
  io_ui_aspect_mode_keep // Keep the aspect ratio (using letter and pillar
                         // boxing)
};
typedef io_uint32_t io_ui_aspect_mode;

//----------------------------------------------------------------------------//
enum io_ui_text_flag_
{
  io_ui_text_flag_wrap = 0x01u // Wrap text
};
typedef io_uint32_t io_ui_text_flag;

//----------------------------------------------------------------------------//
enum io_ui_style_var_
{
  io_ui_style_var_text_color,         // Color for text (vec4)
  io_ui_style_var_text_outline_color, // Color for text outlines (vec4)
  io_ui_style_var_text_outline, // The width (in px) of the text outline (float)
                                // Set to > 0 to add outlines to text
  io_ui_style_var_rect_rounding, // Value in [0, 1] that defines the rounding
                                 // radius. Set to > 0 to draw rectangles with
                                 // rounded corners (float)
  io_ui_style_var_draw_outline,  // Draw outlines with the given width (in px)
                                 // instead of filled shapes
  io_ui_style_var_alpha // Global alpha value applied to all draw operations
                        // (float)
};
typedef io_uint32_t io_ui_style_var;

// Flags used to configure the radius damage applied to the world
//----------------------------------------------------------------------------//
enum io_world_radius_damage_flags_
{
  io_radius_damage_flags_shade_crater =
      0x01u, // Shade the crater around the area affected by the radius damage
  io_radius_damage_flags_fracture =
      0x02u // Apply radius damage as fracture damage
};
typedef io_uint32_t io_world_radius_damage_flags;

// Input key state
//----------------------------------------------------------------------------//
enum io_input_key_state_
{
  io_input_key_state_released,
  io_input_key_state_pressed,
  io_input_key_state_clicked
};
typedef io_uint8_t io_input_key_state;

// Input axis
//----------------------------------------------------------------------------//
enum io_input_axis_
{
  io_input_axis_left_x,
  io_input_axis_left_y,
  io_input_axis_right_x,
  io_input_axis_right_y,

  io_input_axis_trigger_left,
  io_input_axis_trigger_right,

  io_input_axis_invalid
};
typedef io_uint8_t io_input_axis;

// Input key
//----------------------------------------------------------------------------//
enum io_input_key_
{
  io_input_key_up,
  io_input_key_down,
  io_input_key_left,
  io_input_key_right,

  io_input_key_a,
  io_input_key_b,
  io_input_key_c,
  io_input_key_d,
  io_input_key_e,
  io_input_key_f,
  io_input_key_g,
  io_input_key_h,
  io_input_key_i,
  io_input_key_j,
  io_input_key_k,
  io_input_key_l,
  io_input_key_m,
  io_input_key_n,
  io_input_key_o,
  io_input_key_p,
  io_input_key_q,
  io_input_key_r,
  io_input_key_s,
  io_input_key_t,
  io_input_key_u,
  io_input_key_v,
  io_input_key_w,
  io_input_key_x,
  io_input_key_y,
  io_input_key_z,

  io_input_key_0,
  io_input_key_1,
  io_input_key_2,
  io_input_key_3,
  io_input_key_4,
  io_input_key_5,
  io_input_key_6,
  io_input_key_7,
  io_input_key_8,
  io_input_key_9,

  io_input_key_f1,
  io_input_key_f2,
  io_input_key_f3,
  io_input_key_f4,
  io_input_key_f5,
  io_input_key_f6,
  io_input_key_f7,
  io_input_key_f8,
  io_input_key_f9,
  io_input_key_f10,
  io_input_key_f11,
  io_input_key_f12,

  io_input_key_del,
  io_input_key_backspace,
  io_input_key_tab,

  io_input_key_mouse_left,
  io_input_key_mouse_right,
  io_input_key_mouse_middle,

  io_input_key_shift,
  io_input_key_alt,
  io_input_key_ctrl,

  io_input_key_space,
  io_input_key_escape,
  io_input_key_return,

  io_input_key_num_plus,
  io_input_key_num_minus,
  io_input_key_num_0,
  io_input_key_num_1,
  io_input_key_num_2,
  io_input_key_num_3,
  io_input_key_num_4,
  io_input_key_num_5,
  io_input_key_num_6,
  io_input_key_num_7,
  io_input_key_num_8,
  io_input_key_num_9,

  io_input_key_controller_button_a,
  io_input_key_controller_button_y,
  io_input_key_controller_button_b,
  io_input_key_controller_button_x,

  io_input_key_any,
  io_input_key_invalid,

  io_input_key_num_keys
};
typedef io_uint8_t io_input_key;

//----------------------------------------------------------------------------//
// Custom types
//----------------------------------------------------------------------------//

// Ref type
//----------------------------------------------------------------------------//
typedef struct
{
  io_uint32_t internal;
} io_ref_t;

// Name type
//----------------------------------------------------------------------------//
typedef struct
{
  io_uint32_t hash;
} io_name_t;

// 16-bit handle type
//----------------------------------------------------------------------------//
typedef struct
{
  io_uint16_t internal;
} io_handle16_t;

// 32-bit handle type
//----------------------------------------------------------------------------//
typedef struct
{
  io_uint32_t internal;
} io_handle32_t;

// 64-bit handle type
//----------------------------------------------------------------------------//
typedef struct
{
  io_uint64_t internal;
} io_handle64_t;

// Variant type
//----------------------------------------------------------------------------//
typedef struct
{
  io_name_t type;
  io_uint32_t data[4u];
} io_variant_t;

// Property description type
//----------------------------------------------------------------------------//
typedef struct
{
  const char* name;
  const char* type;
  io_property_flags flags;
} io_property_desc_t;

//----------------------------------------------------------------------------//
// Global helper functions and types
//----------------------------------------------------------------------------//

// Fixed time step accumulator.
//
// Example usage:
//
//  // Do this once to initialize the accumulator
//  static io_fixed_step_accumulator accum;
//  io_init(60.0f, &accum);
//
//  // Do this every frame
//  io_accumulator_add(delta_t, &accum);
//
//  while (io_accumulator_step(&accum))
//    pos += vel * accum.delta_t;
typedef struct
{
  io_float32_t update_frequency_in_hz; // The fixed update frequency in Hz
                                       // (steps per second).
  io_float32_t delta_t;                // The delta time (in seconds).
  io_float32_t
      interpolator; // The interpolation factor that needs to be applied to,
                    // e.g., smoothen the visual representation.
  io_float32_t accumulator; // The accumulated time.

} io_fixed_step_accumulator;

// Initializes the provided accumulator. Call this function once when creating a
// new accumulator.
//----------------------------------------------------------------------------//
inline void io_init(io_float32_t update_frequency_in_hz,
                    io_fixed_step_accumulator* accumulator)
{
  accumulator->update_frequency_in_hz = update_frequency_in_hz;
  accumulator->delta_t = 1.0f / accumulator->update_frequency_in_hz;
  accumulator->accumulator = 0.0f;
  accumulator->interpolator = 0.0f;
}

// Call this function once per frame for each accumulator providing the variable
// frame delta time.
//----------------------------------------------------------------------------//
inline void io_accumulator_add(io_float32_t delta_t,
                               io_fixed_step_accumulator* accumulator)
{
  accumulator->accumulator += delta_t;
}

// Returns true if another fixed step should be executed.
//----------------------------------------------------------------------------//
inline io_bool_t io_accumulator_step(io_fixed_step_accumulator* accumulator)
{
  io_bool_t stepped = IO_FALSE;
  if (accumulator->accumulator >= accumulator->delta_t)
  {
    accumulator->accumulator -= accumulator->delta_t;
    stepped = IO_TRUE;
  }

  accumulator->interpolator = accumulator->accumulator / accumulator->delta_t;
  return stepped;
}

// Returns the minimum of x and y
//----------------------------------------------------------------------------//
inline io_uint32_t io_min(io_uint32_t x, io_uint32_t y)
{
  if (x < y)
    return x;
  return y;
}

// Returns the maximum of x and y
//----------------------------------------------------------------------------//
inline io_uint32_t io_max(io_uint32_t x, io_uint32_t y)
{
  if (x > y)
    return x;
  return y;
}

// Simple and fast hash function
//----------------------------------------------------------------------------//
inline io_uint32_t io_hash(const char* data)
{
  if (0 == data || '\0' == *data)
    return 0u;

  io_uint32_t hash = 5381u;
  while (*data)
    hash = ((hash << 5) + hash) + (io_uint8_t)(*data++);

  return hash;
}

// Converts the given string to a name.
//   Please note that the the given string is not being internalized when using
//   this function, hence it won't be possible to request the string for this
//   name. Please use the name related functions provided by the "io_base_t"
//   interface to ensure string internalization
//----------------------------------------------------------------------------//
inline io_name_t io_to_name(const char* string)
{
  io_name_t name;
  name.hash = io_hash(string);
  return name;
}

//----------------------------------------------------------------------------//
// Interface types and typedefs
//----------------------------------------------------------------------------//

// Called when a change to a file was detected
//----------------------------------------------------------------------------//
typedef void (*io_filesystem_on_file_changed_function)(const char* filename);

// Settings for the pathfinding system
//----------------------------------------------------------------------------//
typedef struct
{
  io_uint32_t find_walkable_cell_range; // The range in voxels we're going to
                                        // search for a walkable cell (at the
                                        // start position of the path).
  io_float32_t capsule_radius;          // The capsule radius of the agent.
  io_float32_t capsule_half_height;     // The capsule half height of the agent.
  io_float32_t step_height; // The maximum step the agent is going to take.
  io_float32_t cell_size;   // The size of the cells/voxels we're using for
                            // calculating the path.
  io_uint32_t
      num_max_steps; // The maximum number of iterations to compute per frame.
} io_pathfinding_path_settings;

// Physics overlap result data
//----------------------------------------------------------------------------//
typedef struct
{
  io_bool_t hit; // True if an overlap was detected.

  io_ref_t entity; // The first entity that is overlapping.
} io_physics_overlap_result;

// Physics raycast result data
//----------------------------------------------------------------------------//
typedef struct
{
  io_bool_t hit; // True if a hit was detected.

  io_float32_t distance; // The distance to the hit.
  io_vec3_t position;    // The position of the hit.
  io_vec3_t normal;      // The normal at the hit position.

  io_ref_t entity; // The entity that was hit.
} io_physics_raycast_result;

// A single pixel of a heightmap
//----------------------------------------------------------------------------//
typedef struct
{
  io_uint32_t internal;
} io_terrain_heightmap_pixel;

// Header for a single event
//----------------------------------------------------------------------------//
typedef struct
{
  io_name_t type; // The type of the event.
  io_uint32_t
      data_size_in_bytes; // The size of the data blob attached to this event.
} io_events_header;

// SOA style batch of script data
//----------------------------------------------------------------------------//
typedef struct
{
  void** user_datas; // User data ptrs that can be used to, e.g., store the Lua
                     // state.
  const io_ref_t* entities; // The entities of each of the script components.
  const io_uint32_t* update_intervals; // The update intervals of each of the
                                       // script components.
} io_user_script_batch;

// Defines an anchor for UI transformations
//----------------------------------------------------------------------------//
typedef struct
{
  float anchor, offset;
} io_ui_anchor;

// Defines a set of anchor offsets for UI transformations
//----------------------------------------------------------------------------//
typedef struct
{
  float left, right, top, bottom;
} io_ui_anchor_offsets;

//----------------------------------------------------------------------------//
// Event data types
//----------------------------------------------------------------------------//

// Physics contact event data
// Event type name: "physics_contact"
// Reported by callback: "on_physics_events"
//----------------------------------------------------------------------------//
typedef struct
{
  io_ref_t entity0, entity1; // The entities participating in the contact.
  io_vec3_t pos, impulse;    // Position and impulse of the contact.
} io_events_data_physics_contact;

//----------------------------------------------------------------------------//
// Interface function decls. and implementations
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
inline void
io_pathfinding_init_path_settings(io_pathfinding_path_settings* settings)
{
  settings->find_walkable_cell_range = 8u;
  settings->capsule_radius = 0.2f;
  settings->capsule_half_height = 0.4f;
  settings->step_height = 0.2f;
  settings->cell_size = 0.2f;
  settings->num_max_steps = 128u;
}

//----------------------------------------------------------------------------//
inline io_terrain_heightmap_pixel io_terrain_create_heightmap_pixel(
    io_float32_t height, io_float32_t grass_height, io_uint8_t palette_index)
{
  const io_uint32_t v_height = io_min(height * 255.0f, 255u);
  const io_uint32_t v_grass_height = io_min(grass_height * 255.0f, 255u);
  const io_uint32_t v_palette_index = palette_index;

  io_terrain_heightmap_pixel p;
  p.internal = v_height | (v_grass_height << 8u) | (palette_index << 16u);

  return p;
}

//----------------------------------------------------------------------------//
inline const void* io_events_get_data(const io_events_header* current)
{
  if (current->data_size_in_bytes > 0u)
    return (io_uint8_t*)current + sizeof(io_events_header);
  return 0;
}

//----------------------------------------------------------------------------//
inline const io_events_header*
io_events_get_next(const io_events_header* current, const io_events_header* end)
{
  const io_events_header* next =
      (io_events_header*)((io_uint8_t*)current + sizeof(io_events_header) +
                          current->data_size_in_bytes);
  return next;
}

//----------------------------------------------------------------------------//
#ifdef IO_API_IMPLEMENTATION
//----------------------------------------------------------------------------//

// Empty

//----------------------------------------------------------------------------//
#endif // IO_API_IMPLEMENTATION
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Interfaces that need to be implemented by the plugin providers. Only
// implemented functions are called.
// **Make sure to NULL unused function ptrs before registering the interface!**
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#define IO_USER_TASK_API_NAME "io_user_task_i"
//----------------------------------------------------------------------------//

// Interface for extending the game mode
//----------------------------------------------------------------------------//
struct io_user_task_i
{
  // Called every frame when the game mode is active.
  void (*on_tick)(io_float32_t delta_t);
};

//----------------------------------------------------------------------------//
#define IO_USER_DEBUG_VIEW_API_NAME "io_user_debug_view_i"
//----------------------------------------------------------------------------//

// Interface for providing custom debug views via Dear ImGui. Debug views can by
// cycled through via the F1 key by default in the editor and the game mode
//----------------------------------------------------------------------------//
struct io_user_debug_view_i
{
  // Called when the debug view should be filled using Dear ImGui calls. Called
  // in the context of the current debug view window.
  void (*on_build_debug_view)(io_float32_t delta_t);
};

//----------------------------------------------------------------------------//
#define IO_USER_EDITOR_API_NAME "io_user_editor_i"
//----------------------------------------------------------------------------//

// Interface for extending the editor
//----------------------------------------------------------------------------//
struct io_user_editor_i
{
  // Called when building the "Plugin" menu in the editor's menu bar. Extend the
  // menu here using ImGui::MenuItem() etc.
  void (*on_build_plugin_menu)();

  // Called every frame when the editor is active.
  void (*on_tick)(io_float32_t delta_t);
};

//----------------------------------------------------------------------------//
#define IO_USER_EVENTS_API_NAME "io_user_events_i"
//----------------------------------------------------------------------------//

// Interface for subscribing to events from subsystems
//----------------------------------------------------------------------------//
struct io_user_events_i
{
  // Called when physics related events are ready to be processed.
  void (*on_physics_events)(const io_events_header* begin,
                            const io_events_header* end);
};

//----------------------------------------------------------------------------//
#define IO_USER_SCRIPT_API_NAME "io_user_script_i"
//----------------------------------------------------------------------------//

// Interface for implementing custom scripting backends
//----------------------------------------------------------------------------//
struct io_user_script_i
{
  // Called when a script component is initialized.
  void (*on_init_script)(const char* script_name, io_ref_t entity,
                         io_uint32_t update_interval, void** user_data);
  // Called when a script component is destroyed.
  void (*on_destroy_script)(io_ref_t entity, void** user_data);
  // Called when the given scripts should be ticked.
  void (*on_tick_scripts)(io_float32_t delta_t,
                          const io_user_script_batch* scripts,
                          io_uint32_t scripts_length);
  // Called when the given scripts should be activated.
  void (*on_activate_scripts)(const io_user_script_batch* scripts,
                              io_uint32_t scripts_length);
  // Called when the given scripts should be deactivated.
  void (*on_deactivate_scripts)(const io_user_script_batch* scripts,
                                io_uint32_t scripts_length);
};

//----------------------------------------------------------------------------//
// Interfaces that are provided by IOLITE.
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#define IO_API_MANAGER_API_NAME "io_api_manager_i"
//----------------------------------------------------------------------------//

// The central interface for registering and retrieving API interfaces
//----------------------------------------------------------------------------//
struct io_api_manager_i
{
  // Registers a new API interface for the given name. Multiple interfaces for
  // the same name are allowed.
  void (*register_api)(const char* name, const void* interface);
  // Unregisters the given API inteface.
  void (*unregister_api)(const void* interface);

  // Finds the first API interface for the given name.
  const void* (*find_first)(const char* name);
  // Returns the next API interface for the given type. NULL if none is found.
  const void* (*get_next)(void* interface);
};

//----------------------------------------------------------------------------//
#define IO_BASE_API_NAME "io_base_i"
//----------------------------------------------------------------------------//

// Collection of global functions which are not tied to a certain
// subsystem
//----------------------------------------------------------------------------//
struct io_base_i
{
  // Refs

  // Returns an invalid ref.
  io_ref_t (*ref_invalid)();
  // Returns true if this ref is valid.
  io_bool_t (*ref_is_valid)(io_ref_t ref);
  // Returns the type id for the provided ref.
  io_uint32_t (*ref_get_type_id)(io_ref_t ref);
  // Returns the id for the given ref.
  io_uint32_t (*ref_get_id)(io_ref_t ref);

  // Names

  // Internalizes the given string and returns a name.
  io_name_t (*name_from_string)(const char* string);
  // Returns the internalized string for the given name.
  const char* (*name_get_string)(io_name_t name);

  // Variants

  // Creates a new variant from a float value.
  io_variant_t (*variant_from_float)(io_float32_t value);
  // Gets the value of the variant as a float.
  io_float32_t (*variant_get_float)(io_variant_t variant);

  // Creates a new variant from a signed integer value.
  io_variant_t (*variant_from_int)(io_int32_t value);
  // Gets the value of the variant as a signed integer.
  io_int32_t (*variant_get_int)(io_variant_t variant);

  // Creates a new variant from a unsigned integer value.
  io_variant_t (*variant_from_uint)(io_uint32_t value);
  // Gets the value of the variant as a unsigned integer.
  io_uint32_t (*variant_get_uint)(io_variant_t variant);

  // Creates a new variant from a string.
  io_variant_t (*variant_from_string)(const char* value);
  // Gets the value of the variant as a string.
  const char* (*variant_get_string)(io_variant_t variant);

  // Creates a new variant from a vec2.
  io_variant_t (*variant_from_vec2)(io_vec2_t value);
  // Gets the value of the variant as a vec2.
  io_vec2_t (*variant_get_vec2)(io_variant_t variant);

  // Creates a new variant from a vec3.
  io_variant_t (*variant_from_vec3)(io_vec3_t value);
  // Gets the value of the variant as a vec3.
  io_vec3_t (*variant_get_vec3)(io_variant_t variant);

  // Creates a new variant from a vec4.
  io_variant_t (*variant_from_vec4)(io_vec4_t value);
  // Gets the value of the variant as a vec4.
  io_vec4_t (*variant_get_vec4)(io_variant_t variant);

  // Creates a new variant from a quat.
  io_variant_t (*variant_from_quat)(io_quat_t value);
  // Gets the value of the variant as a quat.
  io_quat_t (*variant_get_quat)(io_variant_t variant);

  // Creates a new variant from a ivec2.
  io_variant_t (*variant_from_ivec2)(io_ivec2_t value);
  // Gets the value of the variant as a ivec2.
  io_ivec2_t (*variant_get_ivec2)(io_variant_t variant);

  // Creates a new variant from a ivec3.
  io_variant_t (*variant_from_ivec3)(io_ivec3_t value);
  // Gets the value of the variant as a ivec3.
  io_ivec3_t (*variant_get_ivec3)(io_variant_t variant);

  // Creates a new variant from a ivec4.
  io_variant_t (*variant_from_ivec4)(io_ivec4_t value);
  // Gets the value of the variant as a ivec4.
  io_ivec4_t (*variant_get_ivec4)(io_variant_t variant);

  // Creates a new variant from a uvec2.
  io_variant_t (*variant_from_uvec2)(io_uvec2_t value);
  // Gets the value of the variant as a uvec2.
  io_uvec2_t (*variant_get_uvec2)(io_variant_t variant);

  // Creates a new variant from a uvec3.
  io_variant_t (*variant_from_uvec3)(io_uvec3_t value);
  // Gets the value of the variant as a uvec3.
  io_uvec3_t (*variant_get_uvec3)(io_variant_t variant);

  // Creates a new variant from a uvec4.
  io_variant_t (*variant_from_uvec4)(io_uvec4_t value);
  // Gets the value of the variant as a uvec4.
  io_uvec4_t (*variant_get_uvec4)(io_variant_t variant);

  // Dear ImGui

  // Needed to use Dear ImGui in a plugin; use in combination with
  // ImGui::SetCurrentContext().
  void* (*imgui_get_context)();
  // Needed to use Dear ImGui in a plugin; use in combination with
  // ImGui::SetAllocatorFunctions().
  void (*imgui_get_allocator_functions)(void** alloc_func, void** free_func);

  // Memory management. Provides a TLSF-backed, threadsafe allocator which
  // features allocation tracking.

  // Allocates a memory area with the given size.
  void* (*mem_allocate)(io_uint32_t size_in_bytes);
  // Allocates a memory area with the given size and alignment.
  void* (*mem_allocate_aligned)(io_uint32_t size_in_bytes,
                                io_uint32_t alignment_in_bytes);
  // Frees the provided memory area.
  void (*mem_free)(void* ptr);
};

//----------------------------------------------------------------------------//
#define IO_LOGGING_API_NAME "io_logging_i"
//----------------------------------------------------------------------------//

// Provides access to the logging subsystem
//----------------------------------------------------------------------------//
struct io_logging_i
{
  // Logs the given message as information.
  void (*log_info)(const char* msg);
  // Logs the given message as a warning.
  void (*log_warning)(const char* msg);
  // Logs the given message as a error.
  void (*log_error)(const char* msg);
};

//----------------------------------------------------------------------------//
#define IO_EDITOR_API_NAME "io_editor_i"
//----------------------------------------------------------------------------//

// Provides access to the editor.
//----------------------------------------------------------------------------//
struct io_editor_i
{
  // Selects the provided node.
  void (*select_node)(io_ref_t node);

  // Returns the first selected node.
  io_ref_t (*get_first_selected_node)();
  // Returns the first selected entity.
  io_ref_t (*get_first_selected_entity)();
};

//----------------------------------------------------------------------------//
#define IO_CUSTOM_COMPONENTS_API_NAME "io_custom_components_i"
//----------------------------------------------------------------------------//

// Interface for managing custom components
//----------------------------------------------------------------------------//
struct io_custom_components_i
{
  // Registering and configuring custom components

  // Requests a new custom component manager to configure.
  io_handle16_t (*request_manager)();
  // Releases and destroys the given custom component manager.
  void (*release_and_destroy_manager)(io_handle16_t manager);

  // Registers a new property with the given name, default value, optional
  // accessor, and flags.
  //   - Registering properties is only allowed *before* calling "init_manager".
  //   - The type of the property is derived from the type of the
  //     "default_value" variant.
  //   - The "accessor" parameter is optional (set to NULL if not needed) and
  //     can point to a *pointer* of the property's type. The accessor needs to
  //     be available while the managar is registered and gets dynamically
  //     updated in case the property memory changes (when the manager runs over
  //     its capacity). Alternatively, the property memory can be queried using
  //     the "get_property_memory" function.
  //   - The accessors will be initialized *after* calling "init_manager".
  void (*register_property)(io_handle16_t manager, const char* name,
                            io_variant_t default_value, void** accessor,
                            io_property_flags flags);

  // Initializes the manager and makes it available under the given (type) name.
  //   Call this *once* after all properties have been registered.
  void (*init_manager)(io_handle16_t manager, const char* type);

  // Returns the type id for this type of component.
  io_uint32_t (*get_type_id)(io_handle16_t manager);

  // Functions to interact with components and their properties

  // Creates a new custom component and attaches it to the provided parent
  // entity.
  io_ref_t (*create_custom_component)(io_handle16_t manager,
                                      io_ref_t parent_entity);
  // Destroys the given custom component.
  void (*destroy_custom_component)(io_handle16_t manager, io_ref_t component);

  // Gets the linear memory for the property with the given name.
  //  Don't cache the returned pointer! The property memory will change when
  //  the manager runs over its current capacity.
  void* (*get_property_memory)(io_handle16_t manager, const char* name);
  // Gets the linear memory storing the entities of the active components.
  io_ref_t* (*get_entity_memory)(io_handle16_t manager);

  // Returns the number of active components.
  io_uint32_t (*get_num_active_components)(io_handle16_t manager);

  // Gets the entity the given component is attached to.
  io_ref_t (*get_entity)(io_handle16_t manager, io_ref_t component);
  // Gets the component for the given entity (if any).
  io_ref_t (*get_component_for_entity)(io_handle16_t manager, io_ref_t entity);
  // Returns true if the provided component is alive.
  io_bool_t (*is_alive)(io_handle16_t manager, io_ref_t component);

  // Converts the given index to a ref.
  io_ref_t (*make_ref)(io_handle16_t manager, io_uint32_t component_index);
  // Convers the given ref to an index.
  io_uint32_t (*make_index)(io_handle16_t manager, io_ref_t component);
};

//----------------------------------------------------------------------------//
#define IO_SETTINGS_API_NAME "io_settings_i"
//----------------------------------------------------------------------------//

// Provides access to the settings subsystem
//----------------------------------------------------------------------------//
struct io_settings_i
{
  // Sets the given boolean setting.
  void (*set_bool)(const char* name, io_bool_t value);
  // Gets the given boolean setting.
  io_bool_t (*get_bool)(const char* name);

  // Sets the given unsigned integer setting.
  void (*set_uint)(const char* name, io_uint32_t value);
  // Gets the given unsigned integer setting.
  io_uint32_t (*get_uint)(const char* name);

  // Sets the given float setting.
  void (*set_float)(const char* name, io_float32_t value);
  // Gets the given float setting.
  io_float32_t (*get_float)(const char* name);

  // Sets the given string setting.
  void (*set_string)(const char* name, const char* value);
  // Gets the given string setting.
  const char* (*get_string)(const char* name);
};

//----------------------------------------------------------------------------//
#define IO_UI_API_NAME "io_ui_i"
//----------------------------------------------------------------------------//

// Provides access to the UI subsystem
//----------------------------------------------------------------------------//
struct io_ui_i
{
  // Draws a rectangle.
  void (*draw_rect)(io_vec4_t color);
  // Draws a circle.
  void (*draw_circle)(io_vec4_t color);
  // Draws a n-sided polygon.
  void (*draw_ngon)(io_vec4_t color, io_uint32_t num_sides);

  // Draws the given image.
  void (*draw_image)(const char* name, io_vec4_t tint);
  // Gets the size of the given image (in px).
  io_vec2_t (*get_image_size)(const char* name);

  // Draws the given text.
  void (*draw_text)(const char* text,
                    io_ui_text_align_horizontal align_horizontal,
                    io_ui_text_align_vertical align_vertical,
                    io_ui_text_flag flags);

  // Pushes the transform defined by the anchors and rotation (in rad).
  void (*push_transform)(io_ui_anchor left, io_ui_anchor right,
                         io_ui_anchor top, io_ui_anchor bottom,
                         io_float32_t rotation);
  // Pushes the transform defined by the anchor preset, offsets, and rotation
  // (in rad).
  void (*push_transform_preset)(io_ui_anchor_preset preset,
                                io_ui_anchor_offsets offsets,
                                io_float32_t rotation);
  // Pops the last transform from the stack and sets it.
  void (*pop_transform)();

  // Calculates the scale and offset for the given base size and according
  // to the aspect mode.
  void (*push_scale_offset_for_base_size)(io_vec2_t base_size,
                                          io_ui_aspect_mode aspect_mode);
  // Pushes the current scale and offset to the stack and activates the given
  // parameters.
  void (*push_scale_offset)(io_float32_t scale, io_vec2_t offset);
  // Pops the last scale and offset from the stack and sets it.
  void (*pop_scale_offset)();

  // Pushes the current style variation float value to the stack and sets the
  // given one.
  void (*push_style_var_float)(io_ui_style_var var, io_float32_t value);
  // Pushes the current style variation vec4 value to the stack and sets the
  // given one.
  void (*push_style_var_vec4)(io_ui_style_var var, io_vec4_t value);
  void (*pop_style_var)();

  // Clips the children of the current transform.
  void (*clip_children)();

  // Pushes the current font size to the stack and sets the given one.
  void (*push_font_size)(io_float32_t size);
  // Pops the last font size from the stack and sets it.
  void (*pop_font_size)();

  // Returns true if the given position (in px) intersects the current
  // transform.
  io_bool_t (*intersects)(io_vec2_t position);
};

//----------------------------------------------------------------------------//
#define IO_WORLD_API_NAME "io_world_i"
//----------------------------------------------------------------------------//

// Provides access to the world subsystem
//----------------------------------------------------------------------------//
struct io_world_i
{
  // Gets the root node of the world.
  io_ref_t (*get_root_node)();
  // Gets the name of the currently loaded world
  const char* (*get_world_name)();

  // Loads the world with the given name.
  void (*load_world)(const char* name);
  // Saves the current world under the given name.
  void (*save_world)(const char* name);

  // Spawns the prefab with the given name.
  io_ref_t (*spawn_prefab)(const char* name);

  // Gets the current time factor.
  io_float32_t (*get_current_time_factor)();
  // Pushes the current time factor to the stack and applies the given one.
  //   Use this to speed up or slow down the game time.
  void (*push_time_factor)(io_float32_t factor);
  // Pops the last time factor from the stack and activates it.
  void (*pop_time_factor)();

  // Applies radius damage with the given parameters at the given position.
  void (*radius_damage)(io_vec3_t pos, io_float32_t radius,
                        io_world_radius_damage_flags flags, float max_hardness);

  // Calculates a ray from the current camera position to the position of the
  // mouse.
  void (*calc_mouse_ray)(io_vec3_t* origin, io_vec3_t* dir);

  // Highlights the given node.
  void (*highlight_node)(io_ref_t node, io_vec4_t color, io_bool_t outline);
};

//----------------------------------------------------------------------------//
#define IO_SAVE_DATA_API_NAME "io_save_data_i"
//----------------------------------------------------------------------------//

// Provides access to the save data subsystem
//----------------------------------------------------------------------------//
struct io_save_data_i
{
  // Saves the provided node hierarchy to the user data directory.
  void (*save_to_user_data)(const char* filename, io_ref_t node);
  // Loads the node hierarchy from the provided file in the user data directory.
  io_ref_t (*load_from_user_data)(const char* filename);
};

//----------------------------------------------------------------------------//
#define IO_PARTICLE_SYSTEM_API_NAME "io_particle_system_i"
//----------------------------------------------------------------------------//

// Provides access to the particle subsystem
//----------------------------------------------------------------------------//
struct io_particle_system_i
{
  // Spawns a particle emitter for the given effect.
  io_handle16_t (*spawn_particle_emitter)(const char* effect_name,
                                          io_vec3_t pos,
                                          io_float32_t lifetime_in_seconds,
                                          io_bool_t adjust_spawn_rate);
  // Sets the spawn rate for the given emitter.
  void (*set_spawn_rate)(io_handle16_t emitter, io_float32_t spawn_rate);
};

//----------------------------------------------------------------------------//
#define IO_INPUT_SYSTEM_API_NAME "io_input_system_i"
//----------------------------------------------------------------------------//

// Provides access to the input subsystem
//----------------------------------------------------------------------------//
struct io_input_system_i
{
  // Gets the state of the given key.
  io_input_key_state (*get_key_state)(io_input_key key, io_uint8_t player_id);
  // Gets the state of the given axis.
  io_float32_t (*get_axis_state)(io_input_axis axis, io_uint8_t player_id);

  // Gets the mouse position in pixels.
  io_vec2_t (*get_mouse_pos)();
  // Gets the position of the mouse in the viewport.
  io_vec2_t (*get_mouse_pos_viewport)();
  // Gets the mouse position relative to the last one (mouse movement).
  io_vec2_t (*get_mouse_pos_relative)();

  // Call this every frame to show the mouse cursor.
  void (*request_mouse_cursor)();
};

//----------------------------------------------------------------------------//
#define IO_TERRAIN_API_NAME "io_terrain_i"
//----------------------------------------------------------------------------//

// Provides access to the terrain subsystem
//----------------------------------------------------------------------------//
struct io_terrain_i
{
  // Generates heightmap based terrain from the provided data.
  void (*generate_from_data)(const io_terrain_heightmap_pixel* heightmap,
                             io_uint32_t size, const char* palette_name,
                             io_float32_t max_height, io_float32_t voxel_size);
  // Generate heightmap based terrain from the provided image.
  void (*generate_from_image)(const char* heightmap_filename,
                              const char* palette_name, io_float32_t max_height,
                              io_float32_t voxel_size);
};

//----------------------------------------------------------------------------//
#define IO_PHYSICS_API_NAME "io_physics_i"
//----------------------------------------------------------------------------//

// Provides access to the physics subsystem
//----------------------------------------------------------------------------//
struct io_physics_i
{
  // Sets the global gravity.
  void (*set_gravity)(io_vec3_t gravity);
  // Gets the global gravity
  io_vec3_t (*get_gravity)();

  // Performs a sphere overlap test.
  io_physics_overlap_result (*overlap_sphere)(io_vec3_t position,
                                              io_float32_t radius);
  // Performs a sphere sweep test in the given direction.
  io_physics_raycast_result (*sweep_sphere)(io_vec3_t position,
                                            io_float32_t radius,
                                            io_vec3_t direction,
                                            io_float32_t distance);
  // Performs a raycast.
  io_physics_raycast_result (*raycast)(io_vec3_t position, io_vec3_t direction,
                                       io_float32_t distance);
};

//----------------------------------------------------------------------------//
#define IO_DEBUG_GEOMETRY_API_NAME "io_debug_geometry_i"
//----------------------------------------------------------------------------//

// Provides access to the debug geometry subsystem
//----------------------------------------------------------------------------//
struct io_debug_geometry_i
{
  // Simple draw functions

  // Draws a line.
  void (*draw_line)(io_vec3_t start, io_vec3_t end, io_vec4_t color,
                    io_bool_t always_in_front);
  // Draws a sphere.
  void (*draw_sphere)(io_vec3_t center, io_float32_t radius, io_vec4_t color,
                      io_bool_t always_in_front);

  // Batched draw functions

  // Draws the given lines.
  //   Line 1: positions[0], positions[1]
  //   Line 2: positions[2], positions[3]
  //   ...
  void (*draw_lines)(io_vec3_t* positions, io_uint32_t num_positions,
                     io_vec4_t color, io_bool_t always_in_front);
  // Draws the given line strip.
  //   Line 1: positions[0], positions[1]
  //   Line 2: positions[1], positions[2]
  //  ...
  void (*draw_line_strip)(io_vec3_t* positions, io_uint32_t num_positions,
                          io_vec4_t color, io_bool_t always_in_front);

  // Draws the given triangles.
  //   Triangle 1: positions[0], positions[1], positions[2]
  //   Triangle 2: positions[3], positions[4], positions[5]
  //   ...
  void (*draw_solid_triangles)(io_vec3_t* positions, io_uint32_t num_positions,
                               io_vec4_t color, io_bool_t always_in_front);
};

//----------------------------------------------------------------------------//
#define IO_SOUND_API_NAME "io_sound_i"
//----------------------------------------------------------------------------//

// Provides access to the sound subsystem
//----------------------------------------------------------------------------//
struct io_sound_i
{
  // Plays the sound effect with the given name.
  io_handle64_t (*play_sound_effect)(const char* effect_name);
  // Stops the given sound effect.
  void (*stop_sound_effect)(io_handle64_t effect_handle);
  // Sets the position of the given sound effect.
  void (*set_position)(io_handle64_t effect_handle, io_vec3_t position);

  // Gets the current audio spectrum.
  void (*get_spectrum)(const io_float32_t** spectrum,
                       io_uint32_t* spectrum_length);
};

//----------------------------------------------------------------------------//
#define IO_PATHFINDING_API_NAME "io_pathfinding_i"
//----------------------------------------------------------------------------//

// Provides access to the pathfinding subsystem
//----------------------------------------------------------------------------//
struct io_pathfinding_i
{
  // Starts finding a path from "start" to "end".
  io_handle16_t (*find_path)(io_vec3_t start, io_vec3_t end,
                             const io_pathfinding_path_settings* settings);

  // Returns true if the given path handle is valid.
  io_bool_t (*is_valid)(io_handle16_t path_handle);

  // Resets the path.
  void (*reset_path)(io_handle16_t path_handle);
  // Destroys the path.
  void (*destroy_path)(io_handle16_t path_handle);

  // Returns true if a valid path has been found.
  io_bool_t (*is_path_found)(io_handle16_t path_handle);
  // Returns the next position on the path.
  io_bool_t (*get_next_position_on_path)(io_handle16_t path_handle,
                                         io_vec3_t* next_position);

  // Draws the given path.
  void (*draw_path)(io_handle16_t path_handle, io_vec4_t color,
                    io_bool_t always_in_front);
  // Draws the debug geometry for the given path.
  void (*draw_debug_geometry)(io_handle16_t path_handle,
                              io_bool_t always_in_front);
};

//----------------------------------------------------------------------------//
#define IO_FILESYSTEM_API_NAME "io_filesystem_i"
//----------------------------------------------------------------------------//

// Provides various filesystem related functions
//----------------------------------------------------------------------------//
struct io_filesystem_i
{
  // Accessing files in data sources

  // Tries to load the given file from the first data source (directory or
  // package) it is available in.
  io_bool_t (*load_file_from_data_source)(const char* filepath,
                                          io_uint8_t* buffer,
                                          io_uint32_t* buffer_length);

  // Filesystem watches

  // Watches the given directory and calls "callback" when a file changes.
  void (*watch_directory)(const char* directory_path,
                          io_filesystem_on_file_changed_function callback);
  // Watches the given directory in *all* data sources and calls "callback" when
  // a file changes.
  void (*watch_data_source_directory)(
      const char* directory_path,
      io_filesystem_on_file_changed_function callback);
  // Removes all watches using the given callback function.
  void (*remove_directory_watch)(io_filesystem_on_file_changed_function);
};

//----------------------------------------------------------------------------//
#define IO_ENTITY_API_NAME "io_entity_i"
//----------------------------------------------------------------------------//

// Provides access to entities
//----------------------------------------------------------------------------//
struct io_entity_i
{
  // Gets the type id for entities.
  io_uint32_t (*get_type_id)();
  // Returns true if the given entity is alive.
  io_bool_t (*is_alive)(io_ref_t entity);

  // Gets the name of the given entity.
  const char* (*get_name)(io_ref_t entity);
  // Returns the linear memory containing all the names for all active entities.
  io_name_t* (*get_name_memory)();

  // Finds the first entity with the given name.
  io_ref_t (*find_first_entity_with_name)(const char* name);
  // Finds all entities with the given name.
  void (*find_entities_with_name)(const char* name, io_ref_t* entities,
                                  io_uint32_t* entities_length);
};

// Shared component interface
//----------------------------------------------------------------------------//

// Base interface all components provide.
//   *Not all functions are provided by all components*
typedef struct
{
  // Returns the type id for this type of component.
  io_uint32_t (*get_type_id)();

  // Creates a new component and attaches it to the provided parent entity.
  io_ref_t (*create)(io_ref_t parent_entity);
  // Destroys the given component.
  void (*destroy)(io_ref_t component);

  // Commits any changes and reloads the internals of the component.
  void (*commit_changes)(io_ref_t component);

  // Returns the number of active components.
  io_uint32_t (*get_num_active_components)();
  // Gets the component for the given entity (if any).
  io_ref_t (*get_component_for_entity)(io_ref_t entity);

  // Returns a ref for the given linear component index.
  io_ref_t (*make_ref)(io_uint32_t component_index);
  // Returns the linear component index for the given ref
  io_uint32_t (*make_index)(io_ref_t component);

  // Returns true if the given component is alive.
  io_bool_t (*is_alive)(io_ref_t component);
  // Gets the entity the given component is attached to.
  io_ref_t (*get_entity)(io_ref_t component);

  // Sets the property of the given component to the provided value.
  void (*set_property)(io_ref_t component, const char* name,
                       io_variant_t value);
  // Gets the property of the given component as a variant.
  io_variant_t (*get_property)(io_ref_t component, const char* name);

  // Returns the linear property memory for the given property for all active
  // components.
  void* (*get_property_memory)(const char* name);
  // Returns the linear memory containing the entities for all active
  // components.
  io_ref_t* (*get_entity_memory)();

  // Returns a list of property descriptions for all the properties the
  // component provides.
  void (*list_properties)(io_property_desc_t* property_descs,
                          io_uint32_t* property_descs_length);
} io_component_base_i;

//----------------------------------------------------------------------------//
#define IO_COMPONENT_NODE_API_NAME "io_component_node_i"
//----------------------------------------------------------------------------//

// Provides access to node components
//----------------------------------------------------------------------------//
struct io_component_node_i
{
  // Base interface functions.
  io_component_base_i base;

  // Creates a new node and attaches it to the current world's root node.
  io_ref_t (*create)(const char* name);
  // Creates a new node and attaches it to the provided parent node.
  io_ref_t (*create_with_parent)(const char* name, io_ref_t parent);

  // Retrieves the parent node (if any).
  io_ref_t (*get_parent)(io_ref_t node);
  // Retrieves the next sibling node (if any).
  io_ref_t (*get_next_sibling)(io_ref_t node);
  // Retrieves the prev. sibling node (if any).
  io_ref_t (*get_prev_sibling)(io_ref_t node);

  // Sets the position for the given node.
  void (*set_position)(io_ref_t node, io_vec3_t pos);
  // Gets the position of the given node.
  io_vec3_t (*get_position)(io_ref_t node);
  // Gets the linear memory containing the positions of all active nodes
  io_vec3_t* (*get_position_memory)();

  // Sets the world position for the given node.
  void (*set_world_position)(io_ref_t node, io_vec3_t pos);
  // Gets the world position of the given node.
  io_vec3_t (*get_world_position)(io_ref_t node);

  // Sets the size for the given node.
  void (*set_size)(io_ref_t node, io_vec3_t size);
  // Gets the size of the given node.
  io_vec3_t (*get_size)(io_ref_t node);
  // Gets the linear memory containing the sizes of all active nodes
  io_vec3_t* (*get_size_memory)();

  // Sets the world size for the given node.
  void (*set_world_size)(io_ref_t node, io_vec3_t size);
  // Gets the world size of the given node.
  io_vec3_t (*get_world_size)(io_ref_t node);

  // Sets the orientation for the given node.
  void (*set_orientation)(io_ref_t node, io_quat_t orient);
  // Gets the orientation of the given node.
  io_quat_t (*get_orientation)(io_ref_t node);
  // Gets the linear memory containing the orientations of all active nodes
  io_quat_t* (*get_orientation_memory)();

  // Sets the world orientation for the given node.
  void (*set_world_orientation)(io_ref_t node, io_quat_t orient);
  // Gets the world orientation of the given node.
  io_quat_t (*get_world_orientation)(io_ref_t node);

  // Transform the given world space position to the local space of the node.
  io_vec3_t (*to_local_space)(io_ref_t node, io_vec3_t pos);
  // Transform the given local space position to world space.
  io_vec3_t (*to_world_space)(io_ref_t node, io_vec3_t pos);
  // Transform the given local space direction to world space.
  io_vec3_t (*to_world_space_direction)(io_ref_t node, io_vec3_t dir);

  // Collects all nodes in the hierarchy (depth first ordering).
  void (*collect_nodes_depth_first)(io_ref_t p_RootNode, io_ref_t* nodes,
                                    io_uint32_t* nodes_length);
  // Collects all nodes in the hierarchy (breadth first ordering).
  void (*collect_nodes_breadth_first)(io_ref_t p_RootNode, io_ref_t* nodes,
                                      io_uint32_t* nodes_length);

  // Updates the transforms of this node and all its child nodes.
  void (*update_transforms)(io_ref_t node);
  // Splits the given set of nodes according to their depth in the hierarchy and
  // updates their transforms in a jobified fashion using the internal task
  // scheduler.
  void (*update_transforms_jobified)(const io_ref_t* node,
                                     io_uint32_t nodes_length);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_CUSTOM_DATA_API_NAME "io_component_custom_data_i"
//----------------------------------------------------------------------------//

// Provides access to custom data components
//----------------------------------------------------------------------------//
struct io_component_custom_data_i
{
  // Base interface functions.
  io_component_base_i base;

  // Gets the value for the given index.
  io_variant_t (*get)(io_ref_t custom_data, io_uint32_t index);
  // Sets the value for the given index.
  void (*set)(io_ref_t custom_data, io_uint32_t index, io_variant_t value);
  // Sets the value for the given index.
  void (*add)(io_ref_t custom_data, io_variant_t value);
  // Removes the value at the given index.
  void (*remove)(io_ref_t custom_data, io_uint32_t index);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_TAG_API_NAME "io_component_tag_api_i"
//----------------------------------------------------------------------------//

// Provides access to tag components
//----------------------------------------------------------------------------//
struct io_component_tag_i
{
  // Base interface functions.
  io_component_base_i base;

  // Finds all entities with the given tag.
  void (*find_entities_with_tag)(const char* tag, io_ref_t* entities,
                                 io_uint32_t* entities_length);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_FLIPBOOK_ANIMATION_API_NAME                               \
  "io_component_flipbook_animation_i"
//----------------------------------------------------------------------------//

// Provides access to flipbook animation components
//----------------------------------------------------------------------------//
struct io_component_flipbook_animation_i
{
  // Base interface functions.
  io_component_base_i base;

  // Starts playing the provided flipbook animation.
  void (*play)(io_ref_t flipbook_animation);
  // Stops playing the provided flipbook animation.
  void (*stop)(io_ref_t flipbook_animation);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_POST_EFFECT_VOLUME_API_NAME                               \
  "io_component_post_effect_volume_i"
//----------------------------------------------------------------------------//

// Provides access to post effect volume components
//----------------------------------------------------------------------------//
struct io_component_post_effect_volume_i
{
  // Base interface functions.
  io_component_base_i base;
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_CAMERA_API_NAME "io_component_camera_i"
//----------------------------------------------------------------------------//

// Provides access to camera components
//----------------------------------------------------------------------------//
struct io_component_camera_i
{
  // Base interface functions.
  io_component_base_i base;
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_SCRIPT_API_NAME "io_component_script_i"
//----------------------------------------------------------------------------//

// Provides access to script components
//----------------------------------------------------------------------------//
struct io_component_script_i
{
  // Base interface functions.
  io_component_base_i base;
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_LIGHT_API_NAME "io_component_light_i"
//----------------------------------------------------------------------------//

// Provides access to light components
//----------------------------------------------------------------------------//
struct io_component_light_i
{
  // Base interface functions.
  io_component_base_i base;
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_VOXEL_SHAPE_API_NAME "io_component_voxel_shape_i"
//----------------------------------------------------------------------------//

// Provides access to voxel shape components
//----------------------------------------------------------------------------//
struct io_component_voxel_shape_i
{
  // Base interface functions.
  io_component_base_i base;

  // Voxel data related functions

  // Sets the given voxel to the palette index (and clamps pos to the dimensions
  // of the shape).
  void (*set)(io_ref_t shape, io_uvec3_t pos, io_uint8_t palette_index);
  // Sets the given voxel to the palette index (**without clamping** pos to the
  // dimensions of the shape).
  void (*set_unsafe)(io_ref_t shape, io_uvec3_t pos, io_uint8_t palette_index);

  // Sets the fracture mask for the given voxel.
  void (*set_fracture_mask)(io_ref_t shape, io_uvec3_t pos, io_bool_t fracture);

  // Sets the volume defined by min and max to the provided palette index.
  void (*fill)(io_ref_t shape, io_uvec3_t min, io_uvec3_t max,
               io_uint8_t palette_index);

  // Gets the palette index for the given voxel coordinate.
  io_uint8_t (*get)(io_ref_t shape, io_uvec3_t pos);

  // Gets the dimensions of the voxel shape.
  io_uvec3_t (*get_dim)(io_ref_t shape);

  // Gets the underyling voxel data.
  // Directly retrieving the data is the most efficient solution for modifying
  // and reading back large chunks of voxels. The data is layed out in memory as
  // follows:
  //   idx = x + y * dim.x + z * dim.x * dim.y
  io_uint8_t* (*get_voxel_data)(io_ref_t shape);

  // Queues the given voxel shape for voxelization. Has to be called after every
  // change to the underlying voxel data to commit the changes and make them
  // visible.
  void (*voxelize)(io_ref_t shape);

  // Physics related functions

  // Applies the given force vector at the center of mass.
  void (*apply_force)(io_ref_t shape, io_vec3_t force);
  // Applies the given torque vector at the center of mass.
  void (*apply_torque)(io_ref_t shape, io_vec3_t torque);

  // Applies the given force vector at the given world position.
  void (*apply_force_at_world_position)(io_ref_t shape, io_vec3_t force,
                                        io_vec3_t position);
  // Applies the given force vector at the given world position.
  void (*apply_force_at_local_position)(io_ref_t shape, io_vec3_t force,
                                        io_vec3_t position);

  // Sets the linear velocity of this shape.
  void (*set_linear_velocity)(io_ref_t shape, io_vec3_t velocity);
  // Gets the linear velocity of this shape.
  io_vec3_t (*get_linear_velocity)(io_ref_t shape);

  // Sets the angular velocity of this shape.
  void (*set_angular_velocity)(io_ref_t shape, io_vec3_t velocity);
  // Gets the angular velocity of this shape.
  io_vec3_t (*get_angular_velocity)(io_ref_t shape);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_CHARACTER_CONTROLLER_API_NAME                             \
  "io_component_character_controller_i"
//----------------------------------------------------------------------------//

// Provides access to character controller components
//----------------------------------------------------------------------------//
struct io_component_character_controller_i
{
  // Base interface functions.
  io_component_base_i base;

  // Moves the character controller.
  void (*move)(io_ref_t controller, io_vec3_t move_vector);
  // Returns true if the character controller is grounded.
  io_bool_t (*is_grounded)(io_ref_t controller);
  // Returns true if the character controller is colliding with its sides.
  io_bool_t (*is_colliding_sides)(io_ref_t controller);
  // Returns true if the upper part of the character controller is colliding.
  io_bool_t (*is_colliding_up)(io_ref_t controller);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_CAMERA_CONTROLLER_API_NAME                                \
  "io_component_camera_controller_i"
//----------------------------------------------------------------------------//

// Provides access to camera controller components
//----------------------------------------------------------------------------//
struct io_component_camera_controller_i
{
  // Base interface functions.
  io_component_base_i base;

  // Sets the node to target for the given controller.
  void (*set_target_node)(io_ref_t controller, io_ref_t node);
  // Sets the euler angles to target for the given controller.
  void (*set_target_euler_angles)(io_ref_t controller, io_vec3_t euler_angles);
  // Gets the euler angles to target for the given controller.
  io_vec3_t (*get_target_euler_angles)(io_ref_t controller);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_PARTICLE_API_NAME "io_component_particle_i"
//----------------------------------------------------------------------------//

// Provides access to particle components
//----------------------------------------------------------------------------//
struct io_component_particle_i
{
  // Base interface functions.
  io_component_base_i base;

  // Returns the emitter handle for the given particle.
  io_handle16_t (*get_emitter_handle)(io_ref_t particle);
};

// Shared resource interface
//----------------------------------------------------------------------------//

// Base interface all resources provide.
//   *Not all functions are provided by all resoures*
typedef struct
{
  // Returns the type id for this type of resource.
  io_uint32_t (*get_type_id)();

  // Creates a new resource with the given name.
  io_ref_t (*create)(const char* name);
  // Destroys the given resource.
  void (*destroy)(io_ref_t resource);

  // Commits any changes and reloads the internals of the resource.
  void (*commit_changes)(io_ref_t component);

  // Returns the number of active resources.
  io_uint32_t (*get_num_active_resources)();

  // Returns a ref for the given linear resource index.
  io_ref_t (*make_ref)(io_uint32_t resource_index);
  // Returns the linear resource index for the given ref
  io_uint32_t (*make_index)(io_ref_t resource);

  // Returns the name of the resource.
  const char* (*get_name)(io_ref_t resource);
  // Returns the linear memory containing all the names for all active
  // resources.
  io_name_t* (*get_name_memory)();

  // Returns true if the given resource is alive.
  io_bool_t (*is_alive)(io_ref_t resource);

  // Sets the property of the given resource to the provided value.
  void (*set_property)(io_ref_t resource, const char* name, io_variant_t value);
  // Gets the property of the given resource as a variant.
  io_variant_t (*get_property)(io_ref_t resource, const char* name);

  // Returns the linear property memory for the given property for all active
  // resources.
  void* (*get_property_memory)(const char* name);

  // Returns a list of property descriptions for all the properties the
  // resource provides.
  void (*list_properties)(io_property_desc_t* property_descs,
                          io_uint32_t* property_descs_length);
} io_resource_base_i;

//----------------------------------------------------------------------------//
#define IO_RESOURCE_PALETTE_API_NAME "io_resource_palette_i"
//----------------------------------------------------------------------------//

// Provides access to palette resources.
//----------------------------------------------------------------------------//
struct io_resource_palette_i
{
  // Base interface functions.
  io_resource_base_i base;

  // Sets the color for the given palette index of the given palette.
  void (*set_color)(io_ref_t palette, io_uint8_t palette_index,
                    io_vec4_t color);
  // Gets the color for the given palette index of the given palette.
  io_vec4_t (*get_color)(io_ref_t palette, io_uint8_t palette_index);

  // Material parameters:
  //   x: Roughness  [0, 1]
  //   y: Metalmask  [0, 1]
  //   z: Hardness   [0, 255]
  //   w: Emissive   [0, FLT_MAX]

  // Sets the material parameters for the given palette index of the given
  // palette.
  void (*set_material_parameters)(io_ref_t palette, io_uint8_t palette_index,
                                  io_vec4_t parameters);

  // Gets the material parameters for the given palette index of the given
  // palette.
  io_vec4_t (*get_material_parameters)(io_ref_t palette,
                                       io_uint8_t palette_index);
};

#endif