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

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

// API
#define IO_USER_VEC2_TYPE glm::vec2
#define IO_USER_VEC3_TYPE glm::vec3
#define IO_USER_VEC4_TYPE glm::vec4
#define IO_USER_QUAT_TYPE glm::quat
#define IO_USER_UVEC2_TYPE glm::uvec2
#define IO_USER_UVEC3_TYPE glm::uvec3
#define IO_USER_UVEC4_TYPE glm::uvec4
#define IO_API_IMPLEMENTATION
#include "iolite_api.h"
#include "terrain_plugin_api.h"
#include "lua_plugin_api.h"

// Globals
//----------------------------------------------------------------------------//
constexpr uint32_t string_buffer_length = 1024u;
static char string_buffer[string_buffer_length];

// Interfaces we use
//----------------------------------------------------------------------------//
static const io_api_manager_i* io_api_manager;
static const io_logging_i* io_logging;
static const io_base_i* io_base;
static const io_entity_i* io_entity;
static const io_ui_i* io_ui;
static const io_filesystem_i* io_filesystem;
static const io_settings_i* io_settings;
static const io_save_data_i* io_save_data;
static const io_world_i* io_world;
static const io_particle_system_i* io_particle_system;
static const io_sound_i* io_sound;
static const io_input_system_i* io_input_system;
static const io_plugin_terrain_i* io_plugin_terrain;
static const io_physics_i* io_physics;
static const io_debug_geometry_i* io_debug_geometry;
static const io_pathfinding_i* io_pathfinding;

static const io_component_node_i* io_component_node;
static const io_component_custom_data_i* io_component_custom_data;
static const io_component_tag_i* io_component_tag;
static const io_component_flipbook_animation_i* io_component_flipbook_animation;
static const io_component_post_effect_volume_i* io_component_post_effect_volume;
static const io_component_camera_i* io_component_camera;
static const io_component_script_i* io_component_script;
static const io_component_voxel_shape_i* io_component_voxel_shape;
static const io_component_light_i* io_component_light;
static const io_component_character_controller_i*
    io_component_character_controller;
static const io_component_camera_controller_i* io_component_camera_controller;
static const io_component_particle_i* io_component_particle;

// Custom data types
//----------------------------------------------------------------------------//
struct lua_physics_contact_event_t
{
  const char* type;
  struct event_data_t
  {
    io_ref_t entity0, entity1;
    io_vec3_t pos, impulse;
  } data;
};

// Interfaces we provide
//----------------------------------------------------------------------------//
static io_user_script_i io_user_script = {};
static io_user_events_i io_user_events = {};

//----------------------------------------------------------------------------//
namespace internal
{

//----------------------------------------------------------------------------//
static std::vector<std::string> queued_world_loads;
static std::vector<io_ref_t> queued_nodes_to_destroy;

//----------------------------------------------------------------------------//
static void queue_load_world(const char* world_name);

//----------------------------------------------------------------------------//
static void queue_destroy_node(io_ref_t node);

//----------------------------------------------------------------------------//
static auto load_script(sol::state& state, const char* filepath)
    -> sol::protected_function;

//----------------------------------------------------------------------------//
static void execute_script(sol::state& state, const char* directory_path,
                           const char* script_name);

//----------------------------------------------------------------------------//
static void script_activate(sol::state& state, io_ref_t entity,
                            uint32_t update_interval);

//----------------------------------------------------------------------------//
static void script_deactivate(sol::state& state, io_ref_t entity);

//----------------------------------------------------------------------------//
static void script_tick(sol::state& state, io_float32_t delta_t,
                        io_ref_t entity);

//----------------------------------------------------------------------------//
static void script_update(sol::state& state, io_float32_t delta_t,
                          io_ref_t entity, uint32_t update_interval);

//----------------------------------------------------------------------------//
static void script_init_state(sol::state& s);
} // namespace internal