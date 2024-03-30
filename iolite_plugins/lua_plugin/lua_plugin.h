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

// STL
#include <limits>
#include <stdio.h>
#include <filesystem>

// Dependencies
#include "glm.hpp"
#include "gtc/quaternion.hpp"
#include "gtc/random.hpp"
#include "gtx/norm.hpp"
#include "gtc/noise.hpp"
#include "sol/sol.hpp"
#include "stb_sprintf.h"

// API
#define IO_USER_VEC2_TYPE glm::vec2
#define IO_USER_VEC3_TYPE glm::vec3
#define IO_USER_VEC4_TYPE glm::vec4
#define IO_USER_QUAT_TYPE glm::quat
#define IO_USER_UVEC2_TYPE glm::uvec2
#define IO_USER_UVEC3_TYPE glm::uvec3
#define IO_USER_U8VEC3_TYPE glm::u8vec3
#define IO_USER_U16VEC3_TYPE glm::u16vec3
#define IO_USER_IVEC3_TYPE glm::ivec3
#define IO_USER_UVEC4_TYPE glm::uvec4
#define IO_API_IMPLEMENTATION
#include "iolite_api.h"
#include "terrain_plugin_api.h"
#include "lua_plugin_api.h"

// Globals
//----------------------------------------------------------------------------//
constexpr uint32_t string_buffer_length = 1024u;
extern char string_buffer[string_buffer_length];

// Interfaces we use
//----------------------------------------------------------------------------//
extern const io_api_manager_i* io_api_manager;
extern const io_logging_i* io_logging;
extern const io_base_i* io_base;
extern const io_entity_i* io_entity;
extern const io_ui_i* io_ui;
extern const io_filesystem_i* io_filesystem;
extern const io_settings_i* io_settings;
extern const io_save_data_i* io_save_data;
extern const io_world_i* io_world;
extern const io_particle_system_i* io_particle_system;
extern const io_animation_system_i* io_animation_system;
extern const io_sound_i* io_sound;
extern const io_input_system_i* io_input_system;
extern const io_plugin_terrain_i* io_plugin_terrain;
extern const io_physics_i* io_physics;
extern const io_debug_geometry_i* io_debug_geometry;
extern const io_pathfinding_i* io_pathfinding;
extern const io_custom_components_i* io_custom_components;

extern const io_component_node_i* io_component_node;
extern const io_component_custom_data_i* io_component_custom_data;
extern const io_component_tag_i* io_component_tag;
extern const io_component_flipbook_animation_i* io_component_flipbook_animation;
extern const io_component_post_effect_volume_i* io_component_post_effect_volume;
extern const io_component_camera_i* io_component_camera;
extern const io_component_voxel_shape_i* io_component_voxel_shape;
extern const io_component_light_i* io_component_light;
extern const io_component_character_controller_i*
    io_component_character_controller;
extern const io_component_camera_controller_i* io_component_camera_controller;
extern const io_component_particle_i* io_component_particle;
extern const io_component_vehicle_i* io_component_vehicle;
extern const io_component_vehicle_wheel_i* io_component_vehicle_wheel;
extern const io_component_joint_i* io_component_joint;

extern const io_resource_palette_i* io_resource_palette;

// Custom types and helpers
//----------------------------------------------------------------------------//
template <typename T> struct lua_array_wrapper_t
{
  typedef T value_type;
  typedef T* iterator;

  inline lua_array_wrapper_t() {}
  inline lua_array_wrapper_t(T* data, io_size_t length)
  {
    begin_ptr = data;
    end_ptr = data + length;
  }

  inline T& operator[](size_t n) { return begin_ptr[n]; }
  inline iterator begin() { return begin_ptr; }
  inline iterator end() { return end_ptr; }
  inline size_t size() { return size_t(end_ptr - begin_ptr); }
  inline size_t max_size() { return size(); }

  T* begin_ptr{};
  T* end_ptr{};
};

// Custom data types
//----------------------------------------------------------------------------//
struct lua_physics_contact_event_t
{
  const char* type;
  struct event_data_t
  {
    io_ref_t entity0, entity1;
    io_vec3_t pos, impulse;
    const char* type;
  } data;
};

//----------------------------------------------------------------------------//
struct lua_user_event_t
{
  const char* type;
  struct event_data_t
  {
    io_ref_t source_entity;
    lua_array_wrapper_t<io_variant_t> variants;
  } data;
};

// Interfaces we provide
//----------------------------------------------------------------------------//
extern io_user_events_i io_user_events;

// Custom components
//----------------------------------------------------------------------------//
extern io_handle16_t script_manager;

// Public functions
//----------------------------------------------------------------------------//

// Initialize the given state.
//----------------------------------------------------------------------------//
void script_init_state(sol::state& s);

// Queues loading of the world with the given name.
//----------------------------------------------------------------------------//
void queue_load_world(const char* world_name);

// Queues the destruction of the given node.
//----------------------------------------------------------------------------//
void queue_destroy_node(io_ref_t node);

// Loads the script from the given filepath.
//----------------------------------------------------------------------------//
auto load_script(sol::state& state, const char* filepath)
    -> sol::protected_function;

// Adds a new event listener for the provided event type.
//----------------------------------------------------------------------------//
void register_event_listener(io_ref_t target_entity, const char* event_type);

// Removes the event listener for the provided event type.
//----------------------------------------------------------------------------//
void unregister_event_listener(io_ref_t target_entity, const char* event_type);

// Posts a new event of a given type from the given source entity with the
// provided variants as payload.
//----------------------------------------------------------------------------//
void post_event(io_ref_t source_entity, const char* event_type,
                io_variant_t* variants, io_size_t variants_length);