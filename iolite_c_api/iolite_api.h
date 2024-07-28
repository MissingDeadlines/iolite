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
            // has failed (dependency not available, etc.)
}

IO_API_EXPORT void IO_API_CALL unload_plugin()
{
  // Clean up here
}
*/

//----------------------------------------------------------------------------//
// Documentation
//----------------------------------------------------------------------------//

// Functions that return ranges of values
// ----
//   Examples: "find_entities_by_name", "collect_nodes_depth_first", etc.
//   Usage:     To retrieve the total buffer length required, set the
//              buffer ptr to nullptr. The length ptr will be updated
//              accordingly. If both the buffer ptr and the length ptr are
//              provided, the function fills the buffer up to the provided
//              length (or less if the total buffer length is smaller) and sets
//              the length to the amount of entries written to the
//              buffer.
//   Note:      It's best to avoid calling functions of this type multiple times
//              and to pre-allocate a sufficiently large buffer upfront.

// Working with event streams
// ----
//   Examples: "on_physics_events", "on_shape_events", etc.
//   Usage:
//
//     const io_events_header_t* event = begin;
//     while (event < end)
//     {
//       // Check type in header and fetch data via "io_events_get_data(event)"
//       // ...
//
//       event = io_events_get_next(event);
//     }

#ifndef INCLUDE_IO_API
#define INCLUDE_IO_API

//----------------------------------------------------------------------------//
// IOLITE API version
//----------------------------------------------------------------------------//

// The major, minor, and bug fix parts of the IOLITE API version
//----------------------------------------------------------------------------//
#define IO_API_VERSION_MAJOR 0
#define IO_API_VERSION_MINOR 5
#define IO_API_VERSION_BUGFIX 0

// The version of the IOLITE API as a single number
//----------------------------------------------------------------------------//
#define IO_API_VERSION                                                         \
  ((IO_API_VERSION_MAJOR << 24) + (IO_API_VERSION_MINOR << 16) +               \
   (IO_API_VERSION_BUGFIX << 8) + 0)

// Helpers for retrieving the major, minor, and bug fix part from the version
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
typedef io_uint32_t io_size_t;

//----------------------------------------------------------------------------//
typedef io_uint64_t io_uuid_t;
typedef io_uint32_t io_ref_type_id_t;
typedef io_uint32_t io_ref_index_t;
typedef io_uint32_t io_ref_id_t;

//----------------------------------------------------------------------------//
// Math related types
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
typedef struct
{
  union
  {
    struct
    {
      io_float32_t x, y;
    };
    struct
    {
      io_float32_t r, g;
    };
    io_float32_t data[2u];
  };
} io_vec2_t;

//----------------------------------------------------------------------------//
typedef struct
{
  union
  {
    struct
    {
      io_float32_t x, y, z;
    };
    struct
    {
      io_float32_t r, g, b;
    };
    io_float32_t data[3u];
  };
} io_vec3_t;

//----------------------------------------------------------------------------//
typedef struct
{
  union
  {
    struct
    {
      io_float32_t x, y, z, w;
    };
    struct
    {
      io_float32_t r, g, b, a;
    };
    io_float32_t data[4u];
  };
} io_vec4_t;

//----------------------------------------------------------------------------//
typedef struct
{
  union
  {
    struct
    {
      io_float32_t w, x, y, z;
    };
    io_float32_t data[4u];
  };
} io_quat_t;

//----------------------------------------------------------------------------//
typedef struct
{
  union
  {
    struct
    {
      io_int32_t x, y;
    };
    io_int32_t data[2u];
  };
} io_ivec2_t;

//----------------------------------------------------------------------------//
typedef struct
{
  union
  {
    struct
    {
      io_int32_t x, y, z;
    };
    io_int32_t data[3u];
  };
} io_ivec3_t;

//----------------------------------------------------------------------------//
typedef struct
{
  union
  {
    struct
    {
      io_int32_t x, y, z, w;
    };
    io_int32_t data[4u];
  };
} io_ivec4_t;

//----------------------------------------------------------------------------//
typedef struct
{
  union
  {
    struct
    {
      io_uint32_t x, y;
    };
    io_uint32_t data[2u];
  };
} io_uvec2_t;

//----------------------------------------------------------------------------//
typedef struct
{
  union
  {
    struct
    {
      io_uint32_t x, y, z;
    };
    io_uint32_t data[3u];
  };
} io_uvec3_t;

//----------------------------------------------------------------------------//
typedef struct
{
  union
  {
    struct
    {
      io_uint8_t x, y, z;
    };
    io_uint8_t data[3u];
  };
} io_u8vec3_t;

//----------------------------------------------------------------------------//
typedef struct
{
  union
  {
    struct
    {
      io_uint16_t x, y, z;
    };
    io_uint16_t data[3u];
  };
} io_u16vec3_t;

//----------------------------------------------------------------------------//
typedef struct
{
  union
  {
    struct
    {
      io_uint32_t x, y, z, w;
    };
    io_uint32_t data[4u];
  };
} io_uvec4_t;

//----------------------------------------------------------------------------//
typedef struct
{
  io_vec3_t center;
  io_float32_t radius;
} io_sphere_t;

//----------------------------------------------------------------------------//
typedef struct
{
  io_vec3_t center;
  io_vec3_t half_extent;
} io_aabb_t;

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
#ifdef IO_USER_U8VEC3_TYPE
//----------------------------------------------------------------------------//
inline io_u8vec3_t io_cvt(IO_USER_U8VEC3_TYPE v) { return {v.x, v.y, v.z}; }
inline IO_USER_U8VEC3_TYPE io_cvt(io_u8vec3_t v) { return {v.x, v.y, v.z}; }
//----------------------------------------------------------------------------//
#endif // IO_USER_U8VEC3_TYPE
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#ifdef IO_USER_U16VEC3_TYPE
//----------------------------------------------------------------------------//
inline io_u16vec3_t io_cvt(IO_USER_U16VEC3_TYPE v) { return {v.x, v.y, v.z}; }
inline IO_USER_U16VEC3_TYPE io_cvt(io_u16vec3_t v) { return {v.x, v.y, v.z}; }
//----------------------------------------------------------------------------//
#endif // IO_USER_U16VEC3_TYPE
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

// Flags specifying which components of a node transform have changed
//----------------------------------------------------------------------------//
enum io_transform_changed_flags_
{
  io_transform_changed_flags_position = 0x01u,
  io_transform_changed_flags_orientation = 0x02u,
  io_transform_changed_flags_size = 0x04u,
};
typedef io_uint8_t io_transform_changed_flags;

// Flags defining the faces of a box
//----------------------------------------------------------------------------//
enum io_box_face_flags_
{
  io_box_face_flags_front = 0x01u,
  io_box_face_flags_back = 0x02u,
  io_box_face_flags_top = 0x04u,
  io_box_face_flags_bottom = 0x08u,
  io_box_face_flags_left = 0x10u,
  io_box_face_flags_right = 0x20u,

  io_box_face_flags_all = 0x7Fu
};
typedef io_uint8_t io_box_face_flags;

// Indices for the faces of a box matching the flags above
//----------------------------------------------------------------------------//
enum io_box_face_index_
{
  io_box_face_index_front,  // Z + 1
  io_box_face_index_back,   // Z - 1
  io_box_face_index_top,    // Y + 1
  io_box_face_index_bottom, // Y - 1
  io_box_face_index_left,   // X - 1
  io_box_face_index_right   // X + 1
};
typedef io_uint8_t io_box_face_index;

// Flags for configuring properties for custom components
//----------------------------------------------------------------------------//
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
  io_radius_damage_flags_fracture = 0x01u, // Apply as fracture
  io_radius_damage_flags_use_noise =
      0x02u // Apply detail using proecudral noise function
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

// The different types supported by variants
//----------------------------------------------------------------------------//
enum io_variant_type_
{
  io_variant_type_float = 0xF71E19B,
  io_variant_type_int = 0xB888030,
  io_variant_type_uint = 0x7C9F0525,
  io_variant_type_uint8 = 0x107FA9FD,
  io_variant_type_uint16 = 0x2074E8EC,
  io_variant_type_uint64 = 0x2074E98F,
  io_variant_type_bool = 0x7C94B391,
  io_variant_type_name = 0x7C9B0C46,
  io_variant_type_vec2 = 0x7C9F7ED5,
  io_variant_type_vec3 = 0x7C9F7ED6,
  io_variant_type_vec4 = 0x7C9F7ED7,
  io_variant_type_quat = 0x7C9D0500,
  io_variant_type_ivec2 = 0xFAD7C5E,
  io_variant_type_ivec3 = 0xFAD7C5F,
  io_variant_type_ivec4 = 0xFAD7C60,
  io_variant_type_uvec2 = 0x1086A26A,
  io_variant_type_uvec3 = 0x1086A26B,
  io_variant_type_u8vec3 = 0x1D0261E3,
  io_variant_type_u16vec3 = 0xA87DC2F2,
  io_variant_type_uvec4 = 0x1086A26C
};

// Various internal constants used for refs
//----------------------------------------------------------------------------//
enum io_ref_internal_
{
  io_ref_internal_max_id = 65534u,        // Max valid ID, 2^16 - 2
  io_ref_internal_max_gen_id = 253u,      // Max valid gen ID, 2^8 - 3
  io_ref_internal_max_type_id = 254u,     // Max valid type ID, 2^8 - 2
  io_ref_internal_invalid_id = 65535u,    // Indicates invalid ID
  io_ref_internal_invalid_gen_id = 255u,  // Indicates invalid generation ID
  io_ref_internal_invalid_type_id = 255u, // Indicates invalid type ID
  io_ref_internal_is_index_gen_id = 254u  // Indicates ID is index
};

//----------------------------------------------------------------------------//
// Data types and type definitions
//----------------------------------------------------------------------------//

// Ref type
//----------------------------------------------------------------------------//
typedef struct
{
  io_uint16_t id;  // ID of the ref
  io_uint8_t gen;  // Generation of the ref
  io_uint8_t type; // Type ID of the ref
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

// Callback function for scheduler tasks
//----------------------------------------------------------------------------//
typedef void (*io_scheduler_callback_t)(io_uvec2_t range, io_uint32_t thread_id,
                                        io_uint32_t sub_task_index, void* task);

// Scheduler tasks provide the possibility to spread a given workload to the
// available worker threads of the internal task scheduler.
//   Example: If there are 100 workloads set and 10 worker threads available,
//            the scheduler (with default settings) divides the task up into 10
//            sub tasks. Each of the sub tasks will be in charge of handling 10
//            workloads. Each sub task calls the provided callback function
//            with a range in [0, 10).
//   Usage:  Use the "io_init_scheduler_task" function to initialize the task
//           and dispatch the task via the functions provided via the
//           "io_base_i" interface.
//----------------------------------------------------------------------------//
typedef struct io_scheduler_task_t_
{
  // The total number of workloads this task handles.
  //  Must be greater than 0.
  io_uint32_t num_workloads;

  // The number of workloads to target per worker thread. Modifying this value
  // can be useful for "uneven" workloads, making it possible for worker threads
  // to "steal" additional workloads.
  //   Example: A value of 1 spreads the workloads evenly by creating one sub
  //            task per available worker thread.
  //            A value of 2 creates two sub tasks per worker thread. If a
  //            worker does not finish in time, another worker can "steal" the
  //            second sub task from it.
  //   Default: 1
  io_uint32_t target_num_sub_tasks_per_worker;

  // Callback function called for each of the internal sub-tasks.
  //   Needs to be valid function pointer provided by the
  //   user.
  io_scheduler_callback_t callback;
  // Ooptional dependency which needs to be completed before running this task.
  //   Default: nullptr
  struct io_scheduler_task_t_* dependency;

  // The number of currently active sub-tasks.
  //   Used internally, required to be initialized to 0.
  io_uint64_t running_sub_task_count;
} io_scheduler_task_t;

//----------------------------------------------------------------------------//
// Global helper functions, types, and type definitions
//----------------------------------------------------------------------------//

// Initializes a task with the given callback for the given amount of tasks
//----------------------------------------------------------------------------//
inline void io_init_scheduler_task(io_scheduler_task_t* task,
                                   io_uint32_t num_tasks,
                                   io_scheduler_callback_t callback)
{
  task->num_workloads = num_tasks;
  task->callback = callback;

  // Defaults
  task->target_num_sub_tasks_per_worker = 1u;
  task->dependency = 0u;

  // Internal
  task->running_sub_task_count = 0u;
}

// Fixed time step accumulator
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
//----------------------------------------------------------------------------//
typedef struct
{
  io_float32_t update_frequency_in_hz; // The fixed update frequency in Hz
                                       // (steps per second).
  io_float32_t delta_t;                // The delta time (in seconds).
  io_float32_t
      interpolator; // The interpolation factor that needs to be applied to,
                    // e.g., smoothen the visual representation.
  io_float32_t accumulator; // The accumulated time.

} io_fixed_step_accumulator_t;

// Initializes the provided accumulator. Call this function once when creating a
// new accumulator.
//----------------------------------------------------------------------------//
inline void
io_init_fixed_step_accumulator(io_fixed_step_accumulator_t* accumulator,
                               io_float32_t update_frequency_in_hz)
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
                               io_fixed_step_accumulator_t* accumulator)
{
  accumulator->accumulator += delta_t;
}

// Returns true if another fixed step should be executed.
//----------------------------------------------------------------------------//
inline io_bool_t io_accumulator_step(io_fixed_step_accumulator_t* accumulator)
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

// Returns the minimum of x and y.
//----------------------------------------------------------------------------//
inline io_uint32_t io_min(io_uint32_t x, io_uint32_t y)
{
  if (x < y)
    return x;
  return y;
}

// Returns the maximum of x and y.
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

//----------------------------------------------------------------------------//
// Name related functions
//----------------------------------------------------------------------------//

// Converts the given string to a name.
//   Note:  The given string is not being internalized when using
//          this function, hence it won't be possible to request the string for
//          this name. Please use the name related functions provided by the
//          "io_base_i" interface to ensure string internalization.
//----------------------------------------------------------------------------//
inline io_name_t io_to_name(const char* string)
{
  io_name_t name;
  name.hash = io_hash(string);
  return name;
}

//----------------------------------------------------------------------------//
inline io_bool_t io_name_is_equal(io_name_t left, io_name_t right)
{
  return left.hash == right.hash;
}

//----------------------------------------------------------------------------//
inline io_bool_t io_name_is_not_equal(io_name_t left, io_name_t right)
{
  return left.hash != right.hash;
}

//----------------------------------------------------------------------------//
// Handle related functions
//----------------------------------------------------------------------------//

// Returns an invalid 16-bit handle.
//----------------------------------------------------------------------------//
inline io_handle16_t io_handle16_invalid()
{
  io_handle16_t handle;
  handle.internal = (io_uint16_t)-1;
  return handle;
}

// Returns an invalid 32-bit handle.
//----------------------------------------------------------------------------//
inline io_handle32_t io_handle32_invalid()
{
  io_handle32_t handle;
  handle.internal = (io_uint32_t)-1;
  return handle;
}

// Returns an invalid 64-bit handle.
//----------------------------------------------------------------------------//
inline io_handle64_t io_handle64_invalid()
{
  io_handle64_t handle;
  handle.internal = (io_uint64_t)-1;
  return handle;
}

// Returns true if the given 16-bit handle is valid.
//----------------------------------------------------------------------------//
inline io_bool_t io_handle16_is_valid(io_handle16_t handle)
{
  return handle.internal != (io_uint16_t)-1;
}

// Returns true if the given 32-bit handle is valid.
//----------------------------------------------------------------------------//
inline io_bool_t io_handle32_is_valid(io_handle32_t handle)
{
  return handle.internal != (io_uint32_t)-1;
}

// Returns true if the given 64-bit handle is valid.
//----------------------------------------------------------------------------//
inline io_bool_t io_handle64_is_valid(io_handle64_t handle)
{
  return handle.internal != (io_uint64_t)-1;
}

//----------------------------------------------------------------------------//
// Ref related functions
//----------------------------------------------------------------------------//

// Returns an invalid ref.
//----------------------------------------------------------------------------//
inline io_ref_t io_ref_invalid()
{
  io_ref_t ref;
  {
    ref.id = io_ref_internal_invalid_id;
    ref.gen = io_ref_internal_invalid_gen_id;
    ref.type = io_ref_internal_invalid_type_id;
  }
  return ref;
}

// Returns true if the given ref is valid.
//----------------------------------------------------------------------------//
inline io_bool_t io_ref_is_valid(io_ref_t ref)
{
  return ref.id != io_ref_internal_invalid_id &&
         ref.gen != io_ref_internal_invalid_gen_id;
}

// Returns true if both refs are equal.
//----------------------------------------------------------------------------//
inline io_bool_t io_ref_is_equal(io_ref_t left, io_ref_t right)
{
  return left.id == right.id && left.gen == right.gen &&
         left.type == right.type;
}

//----------------------------------------------------------------------------//
// Variant related functions
//----------------------------------------------------------------------------//

// Creates a new variant from a float value.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_float(io_float32_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_float;
    *(io_float32_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as a float.
//----------------------------------------------------------------------------//
inline io_float32_t io_variant_get_float(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_float)
    return 0.0f;
  return *(io_float32_t*)variant.data;
}

// Creates a new variant from a signed integer value.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_int(io_int32_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_int;
    *(io_int32_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as a signed integer.
//----------------------------------------------------------------------------//
inline io_int32_t io_variant_get_int(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_int)
    return 0;
  return *(io_int32_t*)variant.data;
}

// Creates a new variant from an unsigned integer value.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_uint(io_uint32_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_uint;
    *(io_uint32_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as an unsigned integer.
//----------------------------------------------------------------------------//
inline io_uint32_t io_variant_get_uint(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_uint)
    return 0u;
  return *(io_uint32_t*)variant.data;
}

// Creates a new variant from a 8-bit unsigned integer value.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_uint8(io_uint8_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_uint8;
    *(io_uint8_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as a 8-bit unsigned integer.
//----------------------------------------------------------------------------//
inline io_uint8_t io_variant_get_uint8(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_uint8)
    return 0u;
  return *(io_uint8_t*)variant.data;
}

// Creates a new variant from a 16-bit unsigned integer value.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_uint16(io_uint16_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_uint16;
    *(io_uint16_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as a 16-bit unsigned integer.
//----------------------------------------------------------------------------//
inline io_uint16_t io_variant_get_uint16(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_uint16)
    return 0u;
  return *(io_uint16_t*)variant.data;
}

// Creates a new variant from a 64-bit unsigned integer value.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_uint64(io_uint64_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_uint64;
    *(io_uint64_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as a 64-bit unsigned integer.
//----------------------------------------------------------------------------//
inline io_uint64_t io_variant_get_uint64(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_uint64)
    return 0ull;
  return *(io_uint64_t*)variant.data;
}

// Creates a new variant from a boolean value.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_bool(io_bool_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_bool;
    *(io_bool_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as a boolean value.
//----------------------------------------------------------------------------//
inline io_bool_t io_variant_get_bool(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_bool)
    return IO_FALSE;
  return *(io_bool_t*)variant.data;
}

// Creates a new variant from a name.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_name(io_name_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_name;
    *(io_name_t*)v.data = value;
  }
  return v;
}
// Creates a new variant from a string.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_string(const char* value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_name;
    *(io_name_t*)v.data = io_to_name(value);
  }
  return v;
}
// Gets the value of the variant as a name.
//----------------------------------------------------------------------------//
inline io_name_t io_variant_get_name(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_name)
  {
    io_name_t name;
    name.hash = 0u;
    return name;
  }
  return *(io_name_t*)variant.data;
}

// Creates a new variant from a vec2.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_vec2(io_vec2_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_vec2;
    *(io_vec2_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as a vec2.
//----------------------------------------------------------------------------//
inline io_vec2_t io_variant_get_vec2(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_vec2)
  {
    io_vec2_t vec;
    vec.x = 0.0f;
    vec.y = 0.0f;
    return vec;
  }
  return *(io_vec2_t*)variant.data;
}

// Creates a new variant from a vec3.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_vec3(io_vec3_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_vec3;
    *(io_vec3_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as a vec3.
//----------------------------------------------------------------------------//
inline io_vec3_t io_variant_get_vec3(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_vec3)
  {
    io_vec3_t vec;
    vec.x = 0.0f;
    vec.y = 0.0f;
    vec.z = 0.0f;
    return vec;
  }
  return *(io_vec3_t*)variant.data;
}

// Creates a new variant from a vec4.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_vec4(io_vec4_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_vec4;
    *(io_vec4_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as a vec4.
//----------------------------------------------------------------------------//
inline io_vec4_t io_variant_get_vec4(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_vec4)
  {
    io_vec4_t vec;
    vec.x = 0.0f;
    vec.y = 0.0f;
    vec.z = 0.0f;
    vec.w = 0.0f;
    return vec;
  }
  return *(io_vec4_t*)variant.data;
}

// Creates a new variant from a quaternion.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_quat(io_quat_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_quat;
    *(io_quat_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as a quaternion.
//----------------------------------------------------------------------------//
inline io_quat_t io_variant_get_quat(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_quat)
  {
    io_quat_t quat;
    quat.w = 0.0f;
    quat.x = 0.0f;
    quat.y = 0.0f;
    quat.z = 0.0f;
    return quat;
  }
  return *(io_quat_t*)variant.data;
}

// Creates a new variant from an ivec2.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_ivec2(io_ivec2_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_ivec2;
    *(io_ivec2_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as an ivec2.
//----------------------------------------------------------------------------//
inline io_ivec2_t io_variant_get_ivec2(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_ivec2)
  {
    io_ivec2_t vec;
    vec.x = 0;
    vec.y = 0;
    return vec;
  }
  return *(io_ivec2_t*)variant.data;
}

// Creates a new variant from an ivec3.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_ivec3(io_ivec3_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_ivec3;
    *(io_ivec3_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as an ivec3.
//----------------------------------------------------------------------------//
inline io_ivec3_t io_variant_get_ivec3(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_ivec3)
  {
    io_ivec3_t vec;
    vec.x = 0;
    vec.y = 0;
    vec.z = 0;
    return vec;
  }
  return *(io_ivec3_t*)variant.data;
}

// Creates a new variant from an ivec4.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_ivec4(io_ivec4_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_ivec4;
    *(io_ivec4_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as an ivec4.
//----------------------------------------------------------------------------//
inline io_ivec4_t io_variant_get_ivec4(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_ivec4)
  {
    io_ivec4_t vec;
    vec.x = 0;
    vec.y = 0;
    vec.z = 0;
    vec.w = 0;
    return vec;
  }
  return *(io_ivec4_t*)variant.data;
}

// Creates a new variant from an uvec2.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_uvec2(io_uvec2_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_uvec2;
    *(io_uvec2_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as an uvec2.
//----------------------------------------------------------------------------//
inline io_uvec2_t io_variant_get_uvec2(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_uvec2)
  {
    io_uvec2_t vec;
    vec.x = 0u;
    vec.y = 0u;
    return vec;
  }
  return *(io_uvec2_t*)variant.data;
}

// Creates a new variant from an uvec3.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_uvec3(io_uvec3_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_uvec3;
    *(io_uvec3_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as an uvec3.
//----------------------------------------------------------------------------//
inline io_uvec3_t io_variant_get_uvec3(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_uvec3)
  {
    io_uvec3_t vec;
    vec.x = 0u;
    vec.y = 0u;
    vec.z = 0u;
    return vec;
  }
  return *(io_uvec3_t*)variant.data;
}

// Creates a new variant from an 8-bit uvec3.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_u8vec3(io_u8vec3_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_u8vec3;
    *(io_u8vec3_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as an 8-bit uvec3.
//----------------------------------------------------------------------------//
inline io_u8vec3_t io_variant_get_u8vec3(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_u8vec3)
  {
    io_u8vec3_t vec;
    vec.x = 0u;
    vec.y = 0u;
    vec.z = 0u;
    return vec;
  }
  return *(io_u8vec3_t*)variant.data;
}

// Creates a new variant from a 16-bit uvec3.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_u16vec3(io_u16vec3_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_u16vec3;
    *(io_u16vec3_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as a 16-bit uvec3.
//----------------------------------------------------------------------------//
inline io_u16vec3_t io_variant_get_u16vec3(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_u16vec3)
  {
    io_u16vec3_t vec;
    vec.x = 0u;
    vec.y = 0u;
    vec.z = 0u;
    return vec;
  }
  return *(io_u16vec3_t*)variant.data;
}

// Creates a new variant from an uvec4.
//----------------------------------------------------------------------------//
inline io_variant_t io_variant_from_uvec4(io_uvec4_t value)
{
  io_variant_t v;
  {
    v.type.hash = io_variant_type_uvec4;
    *(io_uvec4_t*)v.data = value;
  }
  return v;
}
// Gets the value of the variant as an uvec4.
//----------------------------------------------------------------------------//
inline io_uvec4_t io_variant_get_uvec4(io_variant_t variant)
{
  if (variant.type.hash != io_variant_type_uvec4)
  {
    io_uvec4_t vec;
    vec.x = 0u;
    vec.y = 0u;
    vec.z = 0u;
    vec.w = 0u;
    return vec;
  }
  return *(io_uvec4_t*)variant.data;
}

//----------------------------------------------------------------------------//
// Types and type definitions for API interfaces
//----------------------------------------------------------------------------//

// Called when a change to a file was detected.
//----------------------------------------------------------------------------//
typedef void (*io_filesystem_on_file_changed_function)(const char* filename,
                                                       const char* filepath);

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
  io_uint32_t group_mask; // Masking defining the groups to consider for
                          // calculating the path on.
} io_pathfinding_path_settings_t;

// Describes the animation to play via the animation system
//----------------------------------------------------------------------------//
typedef struct
{
  const char* animation_name; // The name of the animation to play

  io_float32_t play_speed;   // The play speed factor, defaults to 1
  io_float32_t blend_weight; // The blend weight, defaults to 1
  io_float32_t
      blend_in_out_duration; // The time (in seconds) to blend in *and* out
  io_float32_t priority; // The priority. Animation with a higher priority are
                         // applied on top
  io_float32_t
      delay; // The delay (in seconds) before the animation starts playing
  io_bool_t looping;               // Set to true if the animation should loop
  io_bool_t restore_when_finished; // Set to true to restore the initial node
                                   // transforms when the animation is finished
} io_animation_system_animation_desc_t;

// Physics overlap result data
//----------------------------------------------------------------------------//
typedef struct
{
  io_bool_t hit; // True if an overlap was detected.

  io_ref_t entity; // The first entity that is overlapping.
} io_physics_overlap_result_t;

// Physics raycast result data
//----------------------------------------------------------------------------//
typedef struct
{
  io_bool_t hit; // True if a hit was detected.

  io_float32_t distance; // The distance to the hit.
  io_vec3_t position;    // The position of the hit.
  io_vec3_t normal;      // The normal at the hit position.

  io_ref_t entity; // The entity that was hit.
} io_physics_raycast_result_t;

// Voxel shape raycast result data
//----------------------------------------------------------------------------//
typedef struct
{
  io_float32_t distance; // The distance to the hit.

  io_vec3_t normal;       // The normal of the hit (in world space).
  io_vec3_t normal_local; // The normal of the hit (in local/voxel space).

  io_ref_t shape;    // The shape hit.
  io_u8vec3_t coord; // The local coordinate of the voxel in the shape
                     // we've hit.
} io_component_voxel_shape_raycast_result_t;

// Header for a single event
//----------------------------------------------------------------------------//
typedef struct
{
  io_name_t type; // The type of the event.
  io_size_t
      data_size_in_bytes; // The size of the data blob attached to this event.
} io_events_header_t;

// Defines an anchor for UI transformations
//----------------------------------------------------------------------------//
typedef struct
{
  float anchor, offset;
} io_ui_anchor_t;

// Defines a set of anchor offsets for UI transformations
//----------------------------------------------------------------------------//
typedef struct
{
  float left, right, top, bottom;
} io_ui_anchor_offsets_t;

// Defines a rectangle.
//----------------------------------------------------------------------------//
typedef struct
{
  io_vec2_t pos;
  io_vec2_t extent;
} io_ui_rect_t;

// Defines a single particle for the custom particle buffer that can be passed
// to the particle system.
//----------------------------------------------------------------------------//
typedef struct
{
  io_vec3_t pos;     // The position of the particle in world space.
  float size;        // The size of the particle in world units.
  float emissive;    // The emissive intensity. Set to zero for non-emissive
                     // particles.
  io_uint32_t color; // The color encoded as a 32-bit value (4x8 unorm).
} io_particle_system_particle_t;

// Collection of Vulkan internal functions
//----------------------------------------------------------------------------//
typedef struct
{
  // Ptr to function of type "PFN_vkGetInstanceProcAddr". Can be used to
  // retrieve other Vulkan functions dynamically
  void* vk_get_instance_proc_addr;
  // Ptr to function of type "PFN_vkGetDeviceProcAddr". Can be used to retrieve
  // other Vulkan functions dynamically
  void* vk_get_device_proc_addr;
} io_low_level_vulkan_functions_t;

// Various (optional) callbacks available for custom components
//----------------------------------------------------------------------------//
typedef struct
{
  // Called when a set of components is created
  void (*on_components_create)(const io_ref_t* components,
                               io_size_t components_length);
  // Called when a set of components is destroyed
  void (*on_components_destroy)(const io_ref_t* components,
                                io_size_t components_length);
} io_custom_components_callbacks_t;

//----------------------------------------------------------------------------//
// Event data types
//----------------------------------------------------------------------------//

// Physics contact event data
//   Event type name:       "physics_contact"
//   Reported by callback:  "on_physics_events"
//----------------------------------------------------------------------------//
typedef struct
{
  io_ref_t entity0, entity1; // The entities participating in the contact.
  io_vec3_t pos, impulse; // Position and impulse of the contact (not available
                          // for triggers).
  io_name_t type;         // Either "touch_found", "touch_lost",
                          // "trigger_touch_found", or "trigger_touch_lost".
} io_events_data_physics_contact_t;

// Voxel shape fracture event data
//   Event type name:       "shape_fractured"
//   Reported by callback:  "on_shape_events"
//----------------------------------------------------------------------------//
typedef struct
{
  io_ref_t base, chunk; // The base and the chunk shape
  io_u8vec3_t origin;   // The origin of the chunk in the coordinate frame
                        // of the base shape
} io_events_data_shape_fractured_t;

// Voxel shape voxelization completed event data
//   Event type name:       "shape_voxelization_completed"
//   Reported by callback:  "on_shape_events"
//----------------------------------------------------------------------------//
typedef struct
{
  io_ref_t shape; // The shape that has been voxelized
} io_events_data_shape_voxelization_completed_t;

// Voxel shape velocity changed event data
//   Event type name:       "shape_linear_velocity_changed" or
//                          "shape_angular_velocity_changed"
//   Reported by callback:  "on_shape_events"
//----------------------------------------------------------------------------//
typedef struct
{
  io_ref_t shape; // The shape with changed velocity
  io_vec3_t velocity,
      delta; // The velocity and the velocity delta of the shape
} io_events_data_shape_velocity_changed_t;

// Node transform changed event data
//   Event type name:       "node_transform_changed" or
//                          "node_transform_changed_by_physics"
//   Reported by callback:  "on_node_events"
//----------------------------------------------------------------------------//
typedef struct
{
  io_ref_t node; // The node with a changed transform
  io_transform_changed_flags
      flags; // Specifies which components of the transform have changed
} io_events_data_node_transform_changed_t;

//----------------------------------------------------------------------------//
// Interface function declarations and implementations
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
inline void
io_pathfinding_init_path_settings(io_pathfinding_path_settings_t* settings)
{
  settings->find_walkable_cell_range = 8u;
  settings->capsule_radius = 0.2f;
  settings->capsule_half_height = 0.4f;
  settings->step_height = 0.2f;
  settings->cell_size = 0.2f;
  settings->num_max_steps = 128u;
  settings->group_mask = 1u; // Shapes only by default
}

//----------------------------------------------------------------------------//
inline void io_animation_system_init_animation_desc(
    io_animation_system_animation_desc_t* desc)
{
  desc->animation_name = ""; // Has to be set by the user

  desc->play_speed = 1.0f;
  desc->blend_weight = 1.0f;
  desc->blend_in_out_duration = 0.0f;
  desc->priority = 0.0f;
  desc->delay = 0.0f;

  desc->looping = IO_FALSE;
  desc->restore_when_finished = IO_FALSE;
}

//----------------------------------------------------------------------------//
inline const void* io_events_get_data(const io_events_header_t* current)
{
  if (current->data_size_in_bytes > 0u)
    return (io_uint8_t*)current + sizeof(io_events_header_t);
  return 0;
}

//----------------------------------------------------------------------------//
inline const io_events_header_t*
io_events_get_next(const io_events_header_t* current)
{
  const io_events_header_t* next =
      (io_events_header_t*)((io_uint8_t*)current + sizeof(io_events_header_t) +
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
struct io_user_task_i // NOLINT
{
  // Called once when the game mode becomes active.
  void (*on_activate)();
  // Called once when the game mode becomes inactive.
  void (*on_deactivate)();

  // Called every frame when the game mode is active.
  void (*on_tick)(io_float32_t delta_t);
  // Called zero or multiple times when the game mode is active.
  //   Please note that the physics scene is advanced at a fixed rate, so the
  //   provided delta time is *constant*.
  void (*on_tick_physics)(io_float32_t delta_t);
};

//----------------------------------------------------------------------------//
#define IO_USER_DEBUG_VIEW_API_NAME "io_user_debug_view_i"
//----------------------------------------------------------------------------//

// Interface for providing custom debug views via Dear ImGui
//   Debug views can be cycled through via the F1 key by default in the editor
//   and the game mode
//----------------------------------------------------------------------------//
struct io_user_debug_view_i // NOLINT
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
struct io_user_editor_i // NOLINT
{
  // Called when building the "Plugin" menu in the editor's menu bar. Extend the
  // menu here using ImGui::MenuItem() etc.
  void (*on_build_plugin_menu)();

  // Called once when the editor becomes active.
  void (*on_activate)();
  // Called once when the editor becomes inactive.
  void (*on_deactivate)();

  // Called every frame when the editor is active.
  void (*on_tick)(io_float32_t delta_t);
};

//----------------------------------------------------------------------------//
#define IO_USER_EDITOR_TOOL_API_NAME "io_user_editor_tool_i"
//----------------------------------------------------------------------------//

// Interface for implementing custom editor tools
//----------------------------------------------------------------------------//
struct io_user_editor_tool_i // NOLINT
{
  // Called when the tool is active and should be updated.
  void (*on_tick)(io_float32_t delta_t);

  // Return the icon for displaying the tool in the editor.
  const char* (*get_icon)();
  // Return the tooltip shown when hovering the tool.
  const char* (*get_tooltip)();
};

//----------------------------------------------------------------------------//
#define IO_USER_DENOISER_API_NAME "io_user_denoiser_i"
//----------------------------------------------------------------------------//

// Interface for providing denoisers applied when exporting path-traced renders
//----------------------------------------------------------------------------//
struct io_user_denoiser_i // NOLINT
{
  // Requests the name of the denoiser.
  const char* (*get_name)();

  // Request to denoise the input data and write the result to output.
  void (*denoise)(io_uint32_t width, io_uint32_t height, const io_vec4_t* input,
                  io_vec4_t* output);
};

//----------------------------------------------------------------------------//
#define IO_USER_EVENTS_API_NAME "io_user_events_i"
//----------------------------------------------------------------------------//

// Interface for subscribing to events from subsystems
//----------------------------------------------------------------------------//
struct io_user_events_i // NOLINT
{
  // Called when physics related events are ready to be processed.
  //   See "Documentation" for usage details.
  void (*on_physics_events)(const io_events_header_t* begin,
                            const io_events_header_t* end);
  // Called when voxel shape related events are ready to be processed.
  //   See "Documentation" for usage details.
  void (*on_shape_events)(const io_events_header_t* begin,
                          const io_events_header_t* end);
  // Called when node related events are ready to be processed.
  //   See "Documentation" for usage details.
  void (*on_node_events)(const io_events_header_t* begin,
                         const io_events_header_t* end);
};

//----------------------------------------------------------------------------//
// Interfaces that are provided by IOLITE.
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#define IO_API_MANAGER_API_NAME "io_api_manager_i"
//----------------------------------------------------------------------------//

// The central interface for registering and retrieving API interfaces
//----------------------------------------------------------------------------//
struct io_api_manager_i // NOLINT
{
  // Registers a new API interface for the given name. Multiple interfaces for
  // the same name are allowed.
  void (*register_api)(const char* name, const void* interface);
  // Unregisters the given API interface.
  void (*unregister_api)(const void* interface);

  // Finds the first API interface for the given name.
  const void* (*find_first)(const char* name);
  // Returns the next API interface for the given type. NULL if none is found.
  const void* (*get_next)(const void* interface);
};

//----------------------------------------------------------------------------//
#define IO_BASE_API_NAME "io_base_i"
//----------------------------------------------------------------------------//

// Collection of global functions which are not tied to a certain
// subsystem
//----------------------------------------------------------------------------//
struct io_base_i // NOLINT
{
  // Names

  // Internalizes the given string and returns a name.
  io_name_t (*name_from_string)(const char* string);
  // Returns the internalized string for the given name.
  const char* (*name_get_string)(io_name_t name);

  // Memory management. Provides a TLSF-backed, thread-safe allocator which
  // features allocation tracking.

  // Allocates a memory area with the given size.
  void* (*mem_allocate)(io_size_t size_in_bytes);
  // Allocates a memory area with the given size and alignment.
  void* (*mem_allocate_aligned)(io_size_t size_in_bytes,
                                io_size_t alignment_in_bytes);
  // Frees the provided memory area.
  void (*mem_free)(void* ptr);

  // Task scheduler

  // Enqueues and kicks the given task
  void (*scheduler_enqueue_task)(io_scheduler_task_t* task);
  // Waits till the given task is completed
  void (*scheduler_wait_for_task)(const io_scheduler_task_t* task);
  // Returns true if the given task is completed
  io_bool_t (*scheduler_is_task_completed)(const io_scheduler_task_t* task);
};

//----------------------------------------------------------------------------//
#define IO_LOGGING_API_NAME "io_logging_i"
//----------------------------------------------------------------------------//

// Provides access to the logging subsystem
//----------------------------------------------------------------------------//
struct io_logging_i // NOLINT
{
  // Logs the given message as information.
  void (*log_info)(const char* msg);
  // Logs the given message as a warning.
  void (*log_warning)(const char* msg);
  // Logs the given message as a error.
  void (*log_error)(const char* msg);

  // Logs the given message, highlighted as a message originating from a plugin.
  void (*log_plugin)(const char* plugin_tag, const char* msg);
};

//----------------------------------------------------------------------------//
#define IO_EDITOR_API_NAME "io_editor_i"
//----------------------------------------------------------------------------//

// Provides access to the editor
//----------------------------------------------------------------------------//
struct io_editor_i // NOLINT
{
  // Selects the provided node.
  //   Pass an invalid ref for the "node" parameter to clear the selection.
  void (*select_node)(io_ref_t node);

  // Returns the first selected node.
  io_ref_t (*get_first_selected_node)();
  // Returns the first selected entity.
  io_ref_t (*get_first_selected_entity)();

  // Pushes the *current* state of the given entity onto the undo/redo stack.
  //   Use this function before editing an entity to enable undoing and redoing
  //   the change. The "skip_serialization" parameter can be used when the
  //   entity in question has just been created and the according undo operation
  //   should delete it again. The "change" parameter expects a short and
  //   meaningful description of the executed change, e.g., "Shape Edited",
  //   "Entity Translated", etc.
  void (*push_undo_redo_state_for_entity)(const char* change, io_ref_t entity,
                                          io_bool_t skip_serialization);
};

//----------------------------------------------------------------------------//
#define IO_CUSTOM_COMPONENTS_API_NAME "io_custom_components_i"
//----------------------------------------------------------------------------//

// Interface for managing and working with custom components
//----------------------------------------------------------------------------//
struct io_custom_components_i // NOLINT
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
  //     be available while the manager is registered and gets dynamically
  //     updated in case the property memory changes (when the manager runs over
  //     its capacity). Alternatively, the property memory can be queried using
  //     the "get_property_memory" function.
  //   - The accessors will be initialized *after* calling "init_manager".
  void (*register_property)(io_handle16_t manager, const char* name,
                            io_variant_t default_value, void** accessor,
                            io_property_flags flags);

  // Registers the given set of callback functions for the given manager.
  void (*register_callbacks)(io_handle16_t manager,
                             const io_custom_components_callbacks_t* callbacks);

  // Initializes the manager and makes it available under the given (type) name.
  //   Call this *once* after all properties have been registered.
  void (*init_manager)(io_handle16_t manager, const char* type);

  // Returns the type ID for this type of component.
  io_ref_type_id_t (*get_type_id)(io_handle16_t manager);

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

  // Gets a pointer to a pointer pointing to the linear memory for the
  // property with the given name. The referenced pointer is updated if the
  // internal memory changes.
  //   It's only safe to cache the returned pointer when all properties have
  //   been registered.
  void** (*get_property_memory_ptr)(io_handle16_t manager, const char* name);

  // Gets the linear memory storing the entities of the active components.
  //  Don't cache the returned pointer! The property memory will change when
  //  the manager runs over its current capacity.
  io_ref_t* (*get_entity_memory)(io_handle16_t manager);
  // Gets a pointer to a pointer pointing to the linear memory storing the
  // entities of the active components. The referenced pointer is updated if the
  // internal memory changes.
  io_ref_t** (*get_entity_memory_ptr)(io_handle16_t manager);

  // Returns the number of active components.
  io_size_t (*get_num_active_components)(io_handle16_t manager);

  // Gets the entity the given component is attached to.
  io_ref_t (*get_entity)(io_handle16_t manager, io_ref_t component);
  // Gets the component for the given entity (if any).
  io_ref_t (*get_component_for_entity)(io_handle16_t manager, io_ref_t entity);
  // Returns true if the provided component is alive.
  io_bool_t (*is_alive)(io_handle16_t manager, io_ref_t component);

  // Converts the given index to a ref.
  io_ref_t (*make_ref)(io_handle16_t manager, io_ref_index_t component_index);
  // Converts the given ref to an index.
  io_ref_index_t (*make_index)(io_handle16_t manager, io_ref_t component);
};

//----------------------------------------------------------------------------//
#define IO_CUSTOM_EVENT_STREAMS_API_NAME "io_custom_event_streams"
//----------------------------------------------------------------------------//

// Interface for managing and working with custom event streams
//----------------------------------------------------------------------------//
struct io_custom_event_streams_i // NOLINT
{
  // Requests a new event stream to use.
  io_handle16_t (*request_event_stream)();
  // Releases and destroys the given event stream.
  void (*release_and_destroy_event_stream)(io_handle16_t stream);

  // Posts a new event to the event stream with an optional data chunk attached
  // to it.
  //   Note: The "event_data" can be nullptr if the "event_data_size_in_bytes"
  //         is also zero.
  void (*post_event)(io_handle16_t stream, const char* event_type,
                     void* event_data, io_uint32_t event_data_size_in_bytes);

  // Posts a new event to the event stream with an uninitialized data chunk of
  // the given size. Returns the pointer to the data chunk which
  // can *then* be initialized by the user. Returns nullptr if the size of the
  // data chunk is zero.
  void* (*post_event_uninitialized)(io_handle16_t stream,
                                    const char* event_type,
                                    io_uint32_t event_data_size_in_bytes);

  // Returns the "begin" and "end" ptr for processing the currently available
  // events.
  //   See "Documentation" for usage details.
  void (*process_events)(io_handle16_t stream, const io_events_header_t** begin,
                         const io_events_header_t** end);

  // Resets the given event stream. Call this when you have fully processed the
  // events.
  void (*reset)(io_handle16_t stream);
};

//----------------------------------------------------------------------------//
#define IO_SETTINGS_API_NAME "io_settings_i"
//----------------------------------------------------------------------------//

// Provides access to the settings subsystem
//----------------------------------------------------------------------------//
struct io_settings_i // NOLINT
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
struct io_ui_i // NOLINT
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
  // Calculates the bounding rectangle for the given text and settings.
  io_ui_rect_t (*calc_text_bounds)(const char* text,
                                   io_ui_text_align_horizontal align_horizontal,
                                   io_ui_text_align_vertical align_vertical,
                                   io_ui_text_flag flags);
  // Returns the bounding rectangle for the last text that has been drawn.
  io_ui_rect_t (*get_last_text_bounds)();

  // Pushes the transform defined by the anchors and rotation (in rad).
  void (*push_transform)(io_ui_anchor_t left, io_ui_anchor_t right,
                         io_ui_anchor_t top, io_ui_anchor_t bottom,
                         io_float32_t rotation);
  // Pushes the transform defined by the anchor preset, offsets, and rotation
  // (in rad).
  void (*push_transform_preset)(io_ui_anchor_preset preset,
                                io_ui_anchor_offsets_t offsets,
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
struct io_world_i // NOLINT
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

  // Retrieves the currently active camera.
  io_ref_t (*get_active_camera)();
  // Activates the given camera.
  void (*activate_camera)(io_ref_t camera);

  // Gets the current time factor.
  io_float32_t (*get_current_time_factor)();
  // Pushes the current time factor to the stack and applies the given one.
  //   Use this to speed up or slow down the game time.
  void (*push_time_factor)(io_float32_t factor);
  // Pops the last time factor from the stack and activates it.
  void (*pop_time_factor)();

  // Applies radius damage with the given parameters at the given position.
  void (*radius_damage)(io_vec3_t pos, io_float32_t radius,
                        io_world_radius_damage_flags flags,
                        io_uint32_t group_mask, float max_hardness);

  // Calculates a ray from the current camera position to the position of the
  // mouse.
  void (*calc_mouse_ray)(io_vec3_t* origin, io_vec3_t* dir);

  // Enables a visual highlight (transparent overlay and/or outline) for the
  // given node. Pass an invalid ref to the "node" parameter to remove the
  // highlight.
  void (*highlight_node)(io_ref_t node, io_vec4_t color, io_bool_t outline);
  // Highlights the given node (transparent overlay and/or outline) once in the
  // current frame. Call this function each frame to keep the highlight visible.
  //   Please note that this function overwrites/resets any available persistent
  //   highlight.
  void (*highlight_node_once)(io_ref_t node, io_vec4_t color,
                              io_bool_t outline);
};

//----------------------------------------------------------------------------//
#define IO_SAVE_DATA_API_NAME "io_save_data_i"
//----------------------------------------------------------------------------//

// Provides access to the save data subsystem
//----------------------------------------------------------------------------//
struct io_save_data_i // NOLINT
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
struct io_particle_system_i // NOLINT
{
  // Spawns a particle emitter for the given effect.
  io_handle16_t (*spawn_particle_emitter)(const char* effect_name,
                                          io_vec3_t position,
                                          io_float32_t lifetime_in_seconds,
                                          io_bool_t adjust_spawn_rate);
  // Despawns the given emitter.
  void (*despawn_particle_emitter)(io_handle16_t emitter);

  // Attaches the given emitter to the given node. Pass an invalid ref to detach
  // it.
  void (*attach_to_node)(io_handle16_t emitter, io_ref_t node);

  // Sets the spawn rate for the given emitter.
  void (*set_spawn_rate)(io_handle16_t emitter, io_float32_t spawn_rate);
  // Sets the position for the given emitter.
  void (*set_position)(io_handle16_t emitter, io_vec3_t position);
  // Sets the scale for the given emitter.
  void (*set_scale)(io_handle16_t emitter, float scale);
  // Sets the scale for the given emitter.
  void (*set_emission_direction)(io_handle16_t emitter, io_vec3_t direction);

  // Sets or updates a custom particle buffer. Pass a nullptr as buffer to unset
  // it.
  void (*set_custom_particle_buffer)(
      const io_particle_system_particle_t* buffer, io_size_t num_particles);
};

//----------------------------------------------------------------------------//
#define IO_INPUT_SYSTEM_API_NAME "io_input_system_i"
//----------------------------------------------------------------------------//

// Provides access to the input subsystem
//----------------------------------------------------------------------------//
struct io_input_system_i // NOLINT
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
#define IO_ANIMATION_SYSTEM_API_NAME "io_animation_system_i"
//----------------------------------------------------------------------------//

// Provides access to the animation subsystem
//----------------------------------------------------------------------------//
struct io_animation_system_i // NOLINT
{
  // Plays the given animation on the provided node and returns the animation
  // instance handle.
  io_handle64_t (*play_animation)(
      io_ref_t node, const io_animation_system_animation_desc_t* desc);

  // Stops the provided animation instance.
  void (*stop_animation)(io_handle64_t instance);
  // Stops all animations instances running on the provided node.
  void (*stop_animations)(io_ref_t node);
  // Stop all animations instances running on the provided node and its
  // hierarchy.
  void (*stop_all_animations)(io_ref_t node);

  // Pauses the given animation.
  void (*pause_animation)(io_handle64_t instance);
  // Resumes the given (paused) animation.
  void (*resume_animation)(io_handle64_t instance);

  // Returns true if the given animation instance is finished.
  //   Please note that finished instances can not be resumed and any operation
  //   on a finished instance equals a NOP.
  io_bool_t (*is_finished)(io_handle64_t instance);

  // Animates the blend weight of the animation instance towards the provided
  // target blend weight. Optionally stops the animation instance after the
  // blending finished.
  void (*blend_in_out)(io_handle64_t instance, io_float32_t target_blend_weight,
                       io_float32_t duration, io_float32_t delay,
                       io_bool_t stop_animation);

  // Returns the current blend weight for the provided animation instance.
  io_float32_t (*get_blend_weight)(io_handle64_t instance);
  // Sets the blend weight for the provided animation instance.
  void (*set_blend_weight)(io_handle64_t instance, io_float32_t blend_weight);

  // Returns the current play speed for the provided animation instance.
  io_float32_t (*get_play_speed)(io_handle64_t instance);
  // Sets the play speed for the provided animation instance.
  void (*set_play_speed)(io_handle64_t instance, io_float32_t play_speed);

  // Returns the current timeline position for the provided animation instance.
  io_float32_t (*get_timeline_position)(io_handle64_t instance);
  // Sets the timeline position for the provided animation instance.
  void (*set_timeline_position)(io_handle64_t instance,
                                io_float32_t timeline_position);
};

//----------------------------------------------------------------------------//
#define IO_PHYSICS_API_NAME "io_physics_i"
//----------------------------------------------------------------------------//

// Provides access to the physics subsystem
//----------------------------------------------------------------------------//
struct io_physics_i // NOLINT
{
  // Sets the global gravity.
  void (*set_gravity)(io_vec3_t gravity);
  // Gets the global gravity
  io_vec3_t (*get_gravity)();

  // Performs a sphere overlap test.
  io_physics_overlap_result_t (*overlap_sphere)(io_vec3_t position,
                                                io_float32_t radius,
                                                io_uint32_t group_mask);
  // Performs a sphere sweep test in the given direction.
  io_physics_raycast_result_t (*sweep_sphere)(io_vec3_t position,
                                              io_float32_t radius,
                                              io_vec3_t direction,
                                              io_float32_t distance,
                                              io_uint32_t group_mask);
  // Performs a raycast.
  io_physics_raycast_result_t (*raycast)(io_vec3_t origin, io_vec3_t direction,
                                         io_float32_t distance,
                                         io_uint32_t group_mask);
};

//----------------------------------------------------------------------------//
#define IO_DEBUG_GEOMETRY_API_NAME "io_debug_geometry_i"
//----------------------------------------------------------------------------//

// Provides access to the debug geometry subsystem
//----------------------------------------------------------------------------//
struct io_debug_geometry_i // NOLINT
{
  // Simple draw functions

  // Draws a line.
  void (*draw_line)(io_vec3_t start, io_vec3_t end, io_vec4_t color,
                    io_bool_t always_in_front);

  // Draws a sphere.
  void (*draw_sphere)(io_vec3_t center, io_float32_t radius, io_vec4_t color,
                      io_bool_t always_in_front);

  // Draws a box.
  void (*draw_box)(io_vec3_t center, io_quat_t orientation, io_vec3_t extent,
                   io_vec4_t color, io_bool_t always_in_front,
                   io_box_face_flags face_flags);
  // Draws a solid box.
  void (*draw_solid_box)(io_vec3_t center, io_quat_t orientation,
                         io_vec3_t extent, io_vec4_t color,
                         io_bool_t always_in_front,
                         io_box_face_flags face_flags);

  // Batched draw functions

  // Draws the given lines.
  //   Line 1: positions[0], positions[1]
  //   Line 2: positions[2], positions[3]
  //   ...
  void (*draw_lines)(io_vec3_t* positions, io_size_t num_positions,
                     io_vec4_t color, io_bool_t always_in_front);
  // Draws the given line strip.
  //   Line 1: positions[0], positions[1]
  //   Line 2: positions[1], positions[2]
  //  ...
  void (*draw_line_strip)(io_vec3_t* positions, io_size_t num_positions,
                          io_vec4_t color, io_bool_t always_in_front);

  // Draws the given triangles.
  //   Triangle 1: positions[0], positions[1], positions[2]
  //   Triangle 2: positions[3], positions[4], positions[5]
  //   ...
  void (*draw_solid_triangles)(io_vec3_t* positions, io_size_t num_positions,
                               io_vec4_t color, io_bool_t always_in_front);

  // Enables software back face culling between the begin/end calls.
  void (*backface_culling_begin)();
  void (*backface_culling_end)();
};

//----------------------------------------------------------------------------//
#define IO_SOUND_API_NAME "io_sound_i"
//----------------------------------------------------------------------------//

// Provides access to the sound subsystem
//----------------------------------------------------------------------------//
struct io_sound_i // NOLINT
{
  // Plays the sound effect with the given name.
  io_handle64_t (*play_sound_effect)(const char* effect_name);
  // Stops the given sound effect.
  void (*stop_sound_effect)(io_handle64_t effect_handle);

  // Sets the position of the given sound effect.
  void (*set_position)(io_handle64_t effect_handle, io_vec3_t position);

  // Gets the current audio spectrum.
  void (*get_spectrum)(const io_float32_t** spectrum,
                       io_size_t* spectrum_length);
};

//----------------------------------------------------------------------------//
#define IO_PATHFINDING_API_NAME "io_pathfinding_i"
//----------------------------------------------------------------------------//

// Provides access to the pathfinding subsystem
//----------------------------------------------------------------------------//
struct io_pathfinding_i // NOLINT
{
  // Starts finding a path from "start" to "end".
  io_handle16_t (*find_path)(io_vec3_t start, io_vec3_t end,
                             const io_pathfinding_path_settings_t* settings);

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

// Provides various file system related functions
//----------------------------------------------------------------------------//
struct io_filesystem_i // NOLINT
{
  // Accessing files in data sources

  // Tries to load the given file from the first data source (directory or
  // package) it is available in.
  //   See "Documentation" for usage details.
  io_bool_t (*load_file_from_data_source)(const char* filepath,
                                          io_uint8_t* buffer,
                                          io_size_t* buffer_length);

  // User data access

  // Creates or retrieves either the user directory or a subdirectory in the
  // user directory. "Subdirectory" is optional and can be NULL.
  void (*create_or_retrieve_user_directory)(const char* subdirectory,
                                            char* buffer,
                                            io_size_t* buffer_length);

  // File system watches

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
struct io_entity_i // NOLINT
{
  // Gets the type ID for entities.
  io_ref_type_id_t (*get_type_id)();
  // Returns true if the given entity is alive.
  io_bool_t (*is_alive)(io_ref_t entity);

  // Gets the name of the given entity.
  const char* (*get_name)(io_ref_t entity);
  // Renames the given entity.
  void (*rename)(io_ref_t entity, const char* name);
  // Returns the linear memory containing all the names for all active entities.
  io_name_t* (*get_name_memory)();

  // Gets the UUID of the given entity.
  io_uuid_t (*get_uuid)(io_ref_t entity);

  // Finds the first entity with the given name.
  io_ref_t (*find_first_entity_with_name)(const char* name);
  // Finds the first entity with the given UUID.
  io_ref_t (*find_entity_with_uuid)(io_uuid_t uuid);

  // Finds all entities with the given name.
  //   See "Documentation" for usage details.
  void (*find_entities_with_name)(const char* name, io_ref_t* entities,
                                  io_size_t* entities_length);
  // Finds all entities which have a component with the given component type
  // name attached.
  void (*find_entities_with_component)(const char* component_type_name,
                                       io_ref_t* entities,
                                       io_size_t* entities_length);

  // Copies and initializes the component of the given type from the source
  // entity to the given target entity.
  //   Rules:
  //     - If the component does not exist on the target entity, a new
  //       component with the properties of the component of the source
  //       entity is created and initialized.
  //     - If the component already exists on the target entity, the
  //       component properties are copied from the source component and
  //       the component gets reinitialized.
  //     - If the component of the given type name does not exist, nothing
  //       is executed.
  //     - If the source component does not have a component of the given type,
  //       nothing is executed
  void (*copy_component_from_entity)(io_ref_t entity_target,
                                     io_ref_t entity_source,
                                     const char* component_type_name);
};

// Base interface all components provide
//   *Not all functions are provided by all components*
//----------------------------------------------------------------------------//
typedef struct
{
  // Returns the type ID for this type of component.
  io_ref_type_id_t (*get_type_id)();

  // Creates a new component and attaches it to the provided parent entity.
  io_ref_t (*create)(io_ref_t parent_entity);
  // Destroys the given component.
  void (*destroy)(io_ref_t component);

  // Commits any changes and reloads the internals of the component.
  void (*commit_changes)(io_ref_t component);

  // Returns the number of active components.
  io_size_t (*get_num_active_components)();
  // Gets the component for the given entity (if any).
  io_ref_t (*get_component_for_entity)(io_ref_t entity);

  // Returns a ref for the given linear component index.
  io_ref_t (*make_ref)(io_ref_index_t component_index);
  // Returns the linear component index for the given ref
  io_ref_index_t (*make_index)(io_ref_t component);

  // Returns true if the given component is alive.
  io_bool_t (*is_alive)(io_ref_t component);
  // Gets the entity the given component is attached to.
  io_ref_t (*get_entity)(io_ref_t component);

  // Sets the property of the given component to the provided value.
  void (*set_property)(io_ref_t component, const char* name,
                       io_variant_t value);
  // Gets the property of the given component as a variant.
  io_variant_t (*get_property)(io_ref_t component, const char* name);

  // Gets the linear memory for the property with the given name.
  //  Don't cache the returned pointer! The property memory will change when
  //  the manager runs over its current capacity.
  void* (*get_property_memory)(const char* name);

  // Gets a pointer to a pointer pointing to the linear memory for the
  // property with the given name. The referenced pointer is updated if the
  // internal memory changes.
  //   It's only safe to cache the returned pointer when all properties have
  //   been registered.
  void** (*get_property_memory_ptr)(const char* name);

  // Returns the linear memory containing the entities for all active
  // components.
  io_ref_t* (*get_entity_memory)();
  // Gets a pointer to a pointer pointing to the linear memory storing the
  // entities of the active components. The referenced pointer is updated if the
  // internal memory changes.
  io_ref_t** (*get_entity_memory_ptr)();

  // Returns a list of property descriptions for all the properties the
  // component provides.
  //   See "Documentation" for usage details.
  void (*list_properties)(io_property_desc_t* property_descs,
                          io_size_t* property_descs_length);
} io_component_base_i;

//----------------------------------------------------------------------------//
#define IO_COMPONENT_NODE_API_NAME "io_component_node_i"
//----------------------------------------------------------------------------//

// Provides access to node components
//----------------------------------------------------------------------------//
struct io_component_node_i // NOLINT
{
  // Base interface functions.
  io_component_base_i base;

  // Creates a new node and attaches it to the current world's root node.
  io_ref_t (*create)(const char* name);
  // Creates a new node and attaches it to the provided parent node.
  //   See the documentation of the "attach" function regarding
  //   the "ignore_parent" parameter.
  io_ref_t (*create_with_parent)(const char* name, io_ref_t parent,
                                 io_bool_t ignore_parent);

  // Attaches the given child node to the provided parent node.
  //   By default, the transform of the parent node is considered so the child
  //   node keeps its world transform. Set the "ignore_parent" parameter to
  //   "true" to avoid this behavior and ignore the parent transform.
  void (*attach)(io_ref_t parent, io_ref_t child, io_bool_t ignore_parent);

  // Returns the depth of the node in the hierarchy.
  //   0: World root node
  //   1: All children of the world root node
  //   2: All children of a child of the world root node
  //   3: ..., and so on
  io_uint32_t (*get_depth)(io_ref_t node);

  // Retrieves the parent node (if any).
  io_ref_t (*get_parent)(io_ref_t node);
  // Retrieves the first child of the node (if any).
  io_ref_t (*get_first_child)(io_ref_t node);
  // Retrieves the next sibling node (if any).
  io_ref_t (*get_next_sibling)(io_ref_t node);
  // Retrieves the previous sibling node (if any).
  io_ref_t (*get_prev_sibling)(io_ref_t node);

  // Gets the bounds of the node in local space (as an axis-aligned bounding
  // box).
  io_aabb_t (*get_local_bounds)(io_ref_t node);
  // Gets the bounds of the node in world space (as an axis-aligned bounding
  // box).
  io_aabb_t (*get_world_bounds)(io_ref_t node);

  // Sets the hidden state of the node.
  void (*set_hidden)(io_ref_t node, io_bool_t hidden);
  // Returns true if the node is hidden, false otherwise.
  io_bool_t (*is_hidden)(io_ref_t node);

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
  // Transform the given world space direction to the local space of the node.
  io_vec3_t (*to_local_space_direction)(io_ref_t node, io_vec3_t dir);
  // Transform the given local space position to world space.
  io_vec3_t (*to_world_space)(io_ref_t node, io_vec3_t pos);
  // Transform the given local space direction to world space.
  io_vec3_t (*to_world_space_direction)(io_ref_t node, io_vec3_t dir);

  // Collects all nodes in the hierarchy (depth first ordering).
  //   See "Documentation" for usage details.
  void (*collect_nodes_depth_first)(io_ref_t root_node, io_ref_t* nodes,
                                    io_size_t* nodes_length);
  // Collects all nodes in the hierarchy (breadth first ordering).
  //   See "Documentation" for usage details.
  void (*collect_nodes_breadth_first)(io_ref_t root_node, io_ref_t* nodes,
                                      io_size_t* nodes_length);

  // Updates the transforms of the node hierarchy.
  void (*update_transforms)(io_ref_t node);
  // Updates the transforms of the node hierarchy in parallel (if possible).
  void (*update_transforms_jobified)(const io_ref_t* nodes,
                                     io_size_t nodes_length);

  // Iterates over all the provided nodes and returns the ones which intersect
  // the given point.
  //   If "use_global_bounds" is set to true, the
  //   compound bounds of a node and all its children are used for
  //   the intersection tests.
  void (*intersect_point)(io_vec3_t point, const io_ref_t* nodes,
                          io_size_t nodes_length, io_ref_t* intersecting_nodes,
                          io_size_t* intersecting_nodes_length,
                          io_bool_t use_global_bounds);
  // Iterates over all the provided nodes and returns the ones which intersect
  // the given axis aligned bounding box (AABB).
  //   If "use_global_bounds" is set to true, the
  //   compound bounds of a node and all its children are used for
  //   the intersection tests.
  void (*intersect_aabb)(io_aabb_t aabb, const io_ref_t* nodes,
                         io_size_t nodes_length, io_ref_t* intersecting_nodes,
                         io_size_t* intersecting_nodes_length,
                         io_bool_t use_global_bounds);
  // Iterates over all the provided nodes and returns the ones which intersect
  // the given sphere.
  //   If "use_global_bounds" is set to true, the
  //   compound bounds of a node and all its children are used for
  //   the intersection tests.
  void (*intersect_sphere)(io_sphere_t sphere, const io_ref_t* nodes,
                           io_size_t nodes_length, io_ref_t* intersecting_nodes,
                           io_size_t* intersecting_nodes_length,
                           io_bool_t use_global_bounds);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_CUSTOM_DATA_API_NAME "io_component_custom_data_i"
//----------------------------------------------------------------------------//

// Provides access to custom data components
//----------------------------------------------------------------------------//
struct io_component_custom_data_i // NOLINT
{
  // Base interface functions.
  io_component_base_i base;

  // Gets the variant value for the given index.
  io_variant_t (*get)(io_ref_t custom_data, io_size_t index);
  // Sets the variant value for the given index.
  void (*set)(io_ref_t custom_data, io_size_t index, io_variant_t value);
  // Adds a new variant value to end of the list.
  void (*add)(io_ref_t custom_data, io_variant_t value);
  // Removes the variant value at the given index.
  void (*remove)(io_ref_t custom_data, io_size_t index);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_TAG_API_NAME "io_component_tag_api_i"
//----------------------------------------------------------------------------//

// Provides access to tag components
//----------------------------------------------------------------------------//
struct io_component_tag_i // NOLINT
{
  // Base interface functions.
  io_component_base_i base;

  // Finds all entities with the given tag.
  //   See "Documentation" for usage details.
  void (*find_entities_with_tag)(const char* tag, io_ref_t* entities,
                                 io_size_t* entities_length);

  // Adds a new tag with the given name.
  void (*add)(io_ref_t tag, const char* tag_name);
  // Removes the tag with the given name (if it exists).
  void (*remove)(io_ref_t tag, const char* tag_name);

  // Returns all tags set for this components
  void (*get_tags)(io_ref_t tag, io_name_t* tags, io_size_t* tags_length);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_FLIPBOOK_ANIMATION_API_NAME                               \
  "io_component_flipbook_animation_i"
//----------------------------------------------------------------------------//

// Provides access to flip book animation components
//----------------------------------------------------------------------------//
struct io_component_flipbook_animation_i // NOLINT
{
  // Base interface functions.
  io_component_base_i base;

  // Starts playing the provided flip book animation.
  void (*play)(io_ref_t flipbook_animation);
  // Stops playing the provided flip book animation.
  void (*stop)(io_ref_t flipbook_animation);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_POST_EFFECT_VOLUME_API_NAME                               \
  "io_component_post_effect_volume_i"
//----------------------------------------------------------------------------//

// Provides access to post effect volume components
//----------------------------------------------------------------------------//
struct io_component_post_effect_volume_i // NOLINT
{
  // Base interface functions.
  io_component_base_i base;
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_CAMERA_API_NAME "io_component_camera_i"
//----------------------------------------------------------------------------//

// Provides access to camera components
//----------------------------------------------------------------------------//
struct io_component_camera_i // NOLINT
{
  // Base interface functions.
  io_component_base_i base;
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_LIGHT_API_NAME "io_component_light_i"
//----------------------------------------------------------------------------//

// Provides access to light components
//----------------------------------------------------------------------------//
struct io_component_light_i // NOLINT
{
  // Base interface functions.
  io_component_base_i base;
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_VOXEL_SHAPE_API_NAME "io_component_voxel_shape_i"
//----------------------------------------------------------------------------//

// Provides access to voxel shape components
//----------------------------------------------------------------------------//
struct io_component_voxel_shape_i // NOLINT
{
  // Base interface functions.
  io_component_base_i base;

  // Various

  // Retrieves the current palette in use for this shape (if any).
  io_ref_t (*get_palette)(io_ref_t shape);
  // Commits the data of the shape to as a snapshot.
  //   Snapshots are used to track the state of shapes which have been
  //   edited during runtime and are serialized when storing prefabs,
  //   worlds, etc.
  void (*commit_snapshot)(io_ref_t shape);

  // Conversion helpers

  // Transforms the given world space position to local/voxel space.
  io_vec3_t (*to_local_space)(io_ref_t shape, io_vec3_t position);
  // Transforms the given world space position to a coordinate in local/voxel
  // space.
  io_ivec3_t (*to_local_coord)(io_ref_t shape, io_vec3_t position);
  // Transforms the given local/voxel space position to world space.
  io_vec3_t (*to_world_space)(io_ref_t shape, io_vec3_t position);

  // Voxel data related functions

  // Sets the given voxel to the palette index (and clamps coordinate to the
  // dimensions of the shape).
  void (*set)(io_ref_t shape, io_u8vec3_t coord, io_uint8_t palette_index);
  // Sets the given voxel to the palette index (**without clamping** the
  // coordinate to the dimensions of the shape).
  void (*set_unsafe)(io_ref_t shape, io_u8vec3_t coord,
                     io_uint8_t palette_index);

  // Marks the given voxel as fractured. Requires "support structures" to
  // be enabled for the shape.
  void (*fracture)(io_ref_t shape, io_u8vec3_t coord);
  // Disconnects the given face of the voxel. Requires "support structures" to
  // be enabled for the shape.
  void (*disconnect)(io_ref_t shape, io_u8vec3_t coord, io_box_face_index face);
  // Returns true if the face of the given voxel is connected to another voxel.
  io_bool_t (*is_connected)(io_ref_t shape, io_u8vec3_t coord,
                            io_box_face_index face);

  // Sets the volume defined by the min and max coordinate to the provided
  // palette index.
  void (*fill)(io_ref_t shape, io_u8vec3_t coord_min, io_u8vec3_t coord_max,
               io_uint8_t palette_index);

  // Shrinks the given shape, so it utilizes the least amount of space.
  void (*compact)(io_ref_t shape);

  // Gets the palette index for the given voxel coordinate.
  io_uint8_t (*get)(io_ref_t shape, io_u8vec3_t coord);

  // Gets the dimensions of the voxel shape.
  io_u16vec3_t (*get_dim)(io_ref_t shape);

  // Gets the underlying voxel data.
  // Directly retrieving the data is the most efficient solution for modifying
  // and reading back large chunks of voxels. The data is laid out in memory as
  // follows:
  //   idx = x + y * dim.x + z * dim.x * dim.y
  io_uint8_t* (*get_voxel_data)(io_ref_t shape);

  // Queues the given voxel shape for voxelization. Has to be called after every
  // change to the underlying voxel data to commit the changes and make them
  // visible.
  void (*voxelize)(io_ref_t shape);
  // Returns true if one (or multiple) voxelization requests for the given shape
  // are pending.
  io_bool_t (*is_voxelization_pending)(io_ref_t shape);

  // Voxel shape queries

  // Performs a raycast against the given shape. The result is optional and can
  // be *NULL*.
  io_bool_t (*raycast)(io_ref_t shape, io_vec3_t origin, io_vec3_t direction,
                       io_float32_t distance,
                       io_component_voxel_shape_raycast_result_t* result);
  // Performs a raycast against the bounds of the given shape. The result is
  // optional and can be *NULL*.
  io_bool_t (*raycast_bounds)(
      io_ref_t shape, io_vec3_t origin, io_vec3_t direction,
      io_float32_t distance, io_bool_t flip_winding,
      io_component_voxel_shape_raycast_result_t* result);
  // Performs a raycast again all shapes in the world. *All parameters* beside
  // the origin, direction, and distance are *optional*.
  io_bool_t (*raycast_global)(io_vec3_t origin, io_vec3_t direction,
                              io_float32_t distance, io_uint32_t group_mask,
                              io_component_voxel_shape_raycast_result_t* result,
                              const io_ref_t* shapes_to_ignore,
                              io_size_t shapes_to_ignore_length);
  // Performs a sphere overlap test against all shapes in the world and returns
  // all candidates.
  //   See "Documentation" for usage details.
  void (*overlap_sphere_global)(io_vec3_t position, io_float32_t radius,
                                io_uint32_t group_mask,
                                io_ref_t* overlapping_shapes,
                                io_size_t* overlapping_shapes_length);

  // Physics related functions

  // Applies the given force vector at the center of mass.
  void (*add_force)(io_ref_t shape, io_vec3_t force);
  // Applies the given torque vector at the center of mass.
  void (*add_torque)(io_ref_t shape, io_vec3_t torque);

  // Applies the given force vector at the given world position.
  void (*add_force_at_world_position)(io_ref_t shape, io_vec3_t force,
                                      io_vec3_t position);
  // Applies the given force vector at the given world position.
  void (*add_force_at_local_position)(io_ref_t shape, io_vec3_t force,
                                      io_vec3_t position);

  // Sets the linear velocity of this shape.
  void (*set_linear_velocity)(io_ref_t shape, io_vec3_t velocity);
  // Gets the linear velocity of this shape.
  io_vec3_t (*get_linear_velocity)(io_ref_t shape);

  // Sets the angular velocity of this shape.
  void (*set_angular_velocity)(io_ref_t shape, io_vec3_t velocity);
  // Gets the angular velocity of this shape.
  io_vec3_t (*get_angular_velocity)(io_ref_t shape);

  // Gets the mass of the shape. Returns zero if not available.
  float (*get_mass)(io_ref_t shape);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_VEHICLE_API_NAME "io_component_vehicle_i"
//----------------------------------------------------------------------------//

// Provides access to vehicle components
//----------------------------------------------------------------------------//
struct io_component_vehicle_i // NOLINT
{
  // Base interface functions.
  io_component_base_i base;
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_VEHICLE_WHEEL_API_NAME "io_component_vehicle_wheel_i"
//----------------------------------------------------------------------------//

// Provides access to vehicle wheel components
//----------------------------------------------------------------------------//
struct io_component_vehicle_wheel_i // NOLINT
{
  // Base interface functions.
  io_component_base_i base;

  // Sets the torque (revolutions per second).
  void (*set_torque)(io_ref_t vehicle_wheel, float torque);
  // Gets the torque (revolutions per second).
  float (*get_torque)(io_ref_t vehicle_wheel);

  // Sets the steering angle (in radians).
  void (*set_steering_angle)(io_ref_t vehicle_wheel, float steering_angle);
  // Gets the steering angle (in radians).
  float (*get_steering_angle)(io_ref_t vehicle_wheel);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_JOINT_API_NAME "io_component_joint_i"
//----------------------------------------------------------------------------//

// Provides access to joint components
//----------------------------------------------------------------------------//
struct io_component_joint_i // NOLINT
{
  // Base interface functions.
  io_component_base_i base;
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_CHARACTER_CONTROLLER_API_NAME                             \
  "io_component_character_controller_i"
//----------------------------------------------------------------------------//

// Provides access to character controller components
//----------------------------------------------------------------------------//
struct io_component_character_controller_i // NOLINT
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

  // Returns the raw (fot) position provided by the underlying physics
  // simulation.
  //   Please note that, in contrast to the node position, this position is
  //   *not* interpolated to provide smooth visuals.
  io_vec3_t (*get_foot_position)(io_ref_t controller);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_CAMERA_CONTROLLER_API_NAME                                \
  "io_component_camera_controller_i"
//----------------------------------------------------------------------------//

// Provides access to camera controller components
//----------------------------------------------------------------------------//
struct io_component_camera_controller_i // NOLINT
{
  // Base interface functions.
  io_component_base_i base;

  // Sets the node to target for the given controller.
  void (*set_target_node)(io_ref_t controller, io_ref_t node);
  // Sets the Euler angles to target for the given controller.
  void (*set_target_euler_angles)(io_ref_t controller, io_vec3_t euler_angles);
  // Gets the Euler angles to target for the given controller.
  io_vec3_t (*get_target_euler_angles)(io_ref_t controller);
};

//----------------------------------------------------------------------------//
#define IO_COMPONENT_PARTICLE_API_NAME "io_component_particle_i"
//----------------------------------------------------------------------------//

// Provides access to particle components
//----------------------------------------------------------------------------//
struct io_component_particle_i // NOLINT
{
  // Base interface functions.
  io_component_base_i base;

  // Returns the emitter handle for the given particle.
  io_handle16_t (*get_emitter_handle)(io_ref_t particle);
};

// Base interface all resources provide
//   *Not all functions are provided by all resources*
//----------------------------------------------------------------------------//
typedef struct
{
  // Returns the type ID for this type of resource.
  io_ref_type_id_t (*get_type_id)();

  // Creates a new resource with the given name.
  io_ref_t (*create)(const char* name);
  // Destroys the given resource.
  void (*destroy)(io_ref_t resource);

  // Commits any changes and reloads the internals of the resource.
  void (*commit_changes)(io_ref_t resource);

  // Finds the first resource with the given name.
  io_ref_t (*find_first_resource_with_name)(const char* name);
  // Finds the first resource with the given name.
  io_ref_t (*find_resource_with_uuid)(io_uuid_t uuid);

  // Returns the number of active resources.
  io_size_t (*get_num_active_resources)();

  // Returns a ref for the given linear resource index.
  io_ref_t (*make_ref)(io_ref_index_t resource_index);
  // Returns the linear resource index for the given ref
  io_ref_index_t (*make_index)(io_ref_t resource);

  // Returns the name of the resource.
  const char* (*get_name)(io_ref_t resource);
  // Returns the UUID of the resource.
  io_uuid_t (*get_uuid)(io_ref_t resource);

  // Returns the linear memory containing all the names for all active
  // resources.
  io_name_t* (*get_name_memory)();

  // Returns true if the given resource is alive.
  io_bool_t (*is_alive)(io_ref_t resource);

  // Sets the property of the given resource to the provided value.
  void (*set_property)(io_ref_t resource, const char* name, io_variant_t value);
  // Gets the property of the given resource as a variant.
  io_variant_t (*get_property)(io_ref_t resource, const char* name);

  // Gets the linear memory for the property with the given name.
  //  Don't cache the returned pointer! The property memory will change when
  //  the manager runs over its current capacity.
  void* (*get_property_memory)(const char* name);

  // Gets a pointer to a pointer pointing to the linear memory for the
  // property with the given name. The referenced pointer is updated if the
  // internal memory changes.
  //   It's only safe to cache the returned pointer when all properties have
  //   been registered.
  void** (*get_property_memory_ptr)(const char* name);

  // Returns a list of property descriptions for all the properties the
  // resource provides.
  void (*list_properties)(io_property_desc_t* property_descs,
                          io_size_t* property_descs_length);
} io_resource_base_i;

//----------------------------------------------------------------------------//
#define IO_RESOURCE_PALETTE_API_NAME "io_resource_palette_i"
//----------------------------------------------------------------------------//

// Provides access to palette resources
//----------------------------------------------------------------------------//
struct io_resource_palette_i // NOLINT
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

//----------------------------------------------------------------------------//
#define IO_LOW_LEVEL_PHYSX_API_NAME "io_low_level_physx_i"
//----------------------------------------------------------------------------//

// Provides direct access to the internal low-level PhysX data structures
//   Use this if you want to directly utilize PhysX in your plugin to add
//   custom behavior and functionality
//----------------------------------------------------------------------------//
struct io_low_level_physx_i // NOLINT
{
  // Returns the ptr to the global "physx::PxPhysics" instance.
  void* (*get_px_physics)();
  // Returns the ptr to the global "physx::PxScene" instance.
  void* (*get_px_scene)();

  // Returns the ptr to the "physx::PxRigidActor" instance for the given shape
  // (component).
  //   Please note:
  //     1. The actor can be *NULL* for shapes with pending voxelization or
  //        disabled collision
  //     2. The actor is replaced after the voxelization for a shape finishes
  //        and the previous one becomes *invalid*
  void* (*get_px_rigid_actor_for_shape)(io_ref_t shape);

  // Returns the tpr to the "physx::PxController" instance for the given
  // character controller (component).
  void* (*get_px_controller_for_cct)(io_ref_t controller);
};

//----------------------------------------------------------------------------//
#define IO_LOW_LEVEL_IMGUI_API_NAME "io_low_level_imgui_i"
//----------------------------------------------------------------------------//

// Provides direct access to the internal low-level Dear ImGui data structures
//   Use this in conjunction with "ImGui::SetCurrentContext()" and
//   "ImGui::SetAllocatorFunctions()" in your plugin
struct io_low_level_imgui_i // NOLINT
{
  // Returns the ptr to the global "ImGui::ImGuiContext" instance
  void* (*get_imgui_context)();
  // Returns the ptrs to the global "ImGuiMemAllocFunc" and "ImGuiMemFreeFunc"
  // functions
  void (*get_imgui_allocator_functions)(void** alloc_func, void** free_func);
};

//----------------------------------------------------------------------------//
#define IO_LOW_LEVEL_VULKAN_API_NAME "io_low_level_vulkan_i"
//----------------------------------------------------------------------------//

// Provides direct access to the internal low-level Vulkan data structures
struct io_low_level_vulkan_i // NOLINT
{
  // Returns the highest version of the API used. Constructed with
  // "VK_MAKE_VERSION".
  io_uint32_t (*get_vk_api_version)();

  // Returns the ptr to the Vulkan physical device of type "VkPhysicalDevice".
  void* (*get_vk_physical_device)();
  // Returns the ptr to the Vulkan device of type "VkDevice".
  void* (*get_vk_device)();
  // Returns the ptr to the Vulkan instance of type "VkInstance".
  void* (*get_vk_instance)();

  // Returns a collection of Vulkan-internal functions.
  void (*get_functions)(io_low_level_vulkan_functions_t* functions);
};

#endif