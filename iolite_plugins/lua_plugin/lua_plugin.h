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

extern const io_component_node_i* io_component_node;
extern const io_component_custom_data_i* io_component_custom_data;
extern const io_component_tag_i* io_component_tag;
extern const io_component_flipbook_animation_i* io_component_flipbook_animation;
extern const io_component_post_effect_volume_i* io_component_post_effect_volume;
extern const io_component_camera_i* io_component_camera;
extern const io_component_script_i* io_component_script;
extern const io_component_voxel_shape_i* io_component_voxel_shape;
extern const io_component_light_i* io_component_light;
extern const io_component_character_controller_i*
    io_component_character_controller;
extern const io_component_camera_controller_i* io_component_camera_controller;
extern const io_component_particle_i* io_component_particle;
extern const io_component_vehicle_i* io_component_vehicle;
extern const io_component_vehicle_wheel_i* io_component_vehicle_wheel;
extern const io_component_joint_i* io_component_joint;

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

// Interfaces we provide
//----------------------------------------------------------------------------//
extern io_user_script_i io_user_script;
extern io_user_events_i io_user_events;

//----------------------------------------------------------------------------//
namespace internal
{

//----------------------------------------------------------------------------//
void queue_load_world(const char* world_name);

//----------------------------------------------------------------------------//
void queue_destroy_node(io_ref_t node);

//----------------------------------------------------------------------------//
auto load_script(sol::state& state, const char* filepath)
    -> sol::protected_function;

//----------------------------------------------------------------------------//
void execute_script(sol::state& state, const char* directory_path,
                    const char* script_name);

//----------------------------------------------------------------------------//
void script_activate(sol::state& state, io_ref_t entity,
                     uint32_t update_interval);

//----------------------------------------------------------------------------//
void script_deactivate(sol::state& state, io_ref_t entity);

//----------------------------------------------------------------------------//
void script_tick(sol::state& state, io_float32_t delta_t, io_ref_t entity);

//----------------------------------------------------------------------------//
void script_update(sol::state& state, io_float32_t delta_t, io_ref_t entity,
                   uint32_t update_interval);

//----------------------------------------------------------------------------//
void script_init_state(sol::state& s);
} // namespace internal