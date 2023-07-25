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

// STL
#include <stdio.h>
#include <vector>

// Dependencies
#include "gtc/constants.hpp"
#include "gtc/random.hpp"
#include "imgui.h"
#include "glm.hpp"
#include "gtc/quaternion.hpp"
#include "gtx/norm.hpp"

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

// Interfaces we use
//----------------------------------------------------------------------------//
static const io_api_manager_i* io_api_manager;
static const io_logging_i* io_logging;
static const io_base_i* io_base;
static const io_entity_i* io_entity;
static const io_ui_i* io_ui;
static const io_input_system_i* io_input_system;
static const io_component_node_i* io_component_node;
static const io_component_voxel_shape_i* io_component_voxel_shape;
static const io_editor_i* io_editor;
static const io_custom_components_i* io_custom_components;
static const io_resource_palette_i* io_resource_palette;
static const io_debug_geometry_i* io_debug_geometry;

// Interfaces we provide
//----------------------------------------------------------------------------//
static io_user_task_i io_user_task = {};
static io_user_editor_i io_user_editor = {};
static io_user_debug_view_i io_user_debug_view = {};

// Globals
//----------------------------------------------------------------------------//
static io_handle16_t boid_component_mgr = {};

// Boid component example
//----------------------------------------------------------------------------//
static glm::vec3* comp_boid_position = nullptr;
static glm::vec3* comp_boid_prev_position = nullptr;
static glm::vec3* comp_boid_velocity = nullptr;
static io_fixed_step_accumulator fixed_accum;

//----------------------------------------------------------------------------//
static void simulate_boids()
{
  const io_uint32_t num_boids =
      io_custom_components->get_num_active_components(boid_component_mgr);

  // Need at least two boids
  if (num_boids <= 1)
    return;

  for (io_uint32_t i = 0u; i < num_boids; ++i)
  {
    glm::vec3& vel = comp_boid_velocity[i];

    // Compute center of mass and average velocity
    glm::vec3 center_of_mass = glm::vec3(0.0f);
    glm::vec3 avg_velocity = glm::vec3(0.0f);
    for (io_uint32_t j = 0u; j < num_boids; ++j)
    {
      if (i == j)
        continue;

      center_of_mass += comp_boid_position[j];
      avg_velocity += comp_boid_velocity[j];
    }
    center_of_mass /= num_boids - 1u;
    avg_velocity /= num_boids - 1u;

    // Rule 1: Move towards center of mass
    vel +=
        (center_of_mass - comp_boid_position[i]) * fixed_accum.delta_t * 0.1f;

    // Rule 2: Keep a distance
    for (io_uint32_t j = 0u; j < num_boids; ++j)
    {
      if (i == j)
        continue;

      if (glm::length2(comp_boid_position[j] - comp_boid_position[i]) < 1.0f)
      {
        vel -= (comp_boid_position[j] - comp_boid_position[i]) *
               fixed_accum.delta_t * 0.1f;
      }
    }

    // Finally update our position
    comp_boid_prev_position[i] = comp_boid_position[i];
    comp_boid_position[i] += vel * fixed_accum.delta_t;
  }
}

//----------------------------------------------------------------------------//
static void draw_boids()
{
  const io_uint32_t num_boids =
      io_custom_components->get_num_active_components(boid_component_mgr);

  for (io_uint32_t i = 0u; i < num_boids; ++i)
  {
    // Calculate interpolated position for rendering
    const glm::vec3 pos =
        glm::mix(comp_boid_prev_position[i], comp_boid_position[i],
                 fixed_accum.interpolator);

    io_debug_geometry->draw_sphere(
        io_cvt(pos), 0.1f, io_cvt(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)), false);
  }
}

//----------------------------------------------------------------------------//
static void on_build_plugin_menu()
{
  // Add a new menu item to the menu
  if (ImGui::MenuItem("Spawn Voxel Cubes"))
  {
    constexpr io_uint32_t numNodes = 500u;

    // Create a new palette and set a single blueish color
    const io_ref_t palette = io_resource_palette->base.create("my_palette");
    io_resource_palette->set_color(palette, 1u, {0.5f, 0.5f, 1.0f, 1.0f});
    io_resource_palette->set_material_parameters(palette, 1u,
                                                 {1.0f, 0.0f, 0.0f, 0.0f});
    io_resource_palette->base.commit_changes(palette);

    // Spawn voxel cubes
    for (uint32_t i = 0u; i < numNodes; ++i)
    {
      // Create a new node (and entity)
      const io_ref_t node = io_component_node->create("cube");

      // Generate and set a random position for our cube
      const io_vec3_t initial_position = io_cvt(glm::ballRand(10.0f));
      io_component_node->set_position(node, initial_position);
      io_component_node->update_transforms(node);

      // Create a new voxel shape component and attach it to the entity of our
      // node
      const io_ref_t entity = io_component_node->base.get_entity(node);
      const io_ref_t shape = io_component_voxel_shape->base.create(entity);

      // Set our shape to a cube voxel asset
      io_component_voxel_shape->base.set_property(
          shape, "VoxelShapeName", io_base->variant_from_string("cube_1m"));
      // Set our custom palette
      io_component_voxel_shape->base.set_property(
          shape, "PaletteName", io_base->variant_from_string("my_palette"));
      io_component_voxel_shape->base.commit_changes(shape);
    }
  }

  // Add a new menu item to the menu
  if (ImGui::MenuItem("Spawn Boids"))
  {
    constexpr io_uint32_t numBoids = 2000u;

    // Spawn boids
    for (uint32_t i = 0u; i < numBoids; ++i)
    {
      // Create a new node (and entity)
      const io_ref_t node = io_component_node->create("boid");
      const io_ref_t entity = io_component_node->base.get_entity(node);

      const io_ref_t boid = io_custom_components->create_custom_component(
          boid_component_mgr, entity);
      const io_uint32_t boid_idx =
          io_custom_components->make_index(boid_component_mgr, boid);

      // Assign random position
      comp_boid_position[boid_idx] = comp_boid_prev_position[boid_idx] =
          glm::ballRand(10.0f);
    }
  }
}

//----------------------------------------------------------------------------//
static void on_editor_tick(io_float32_t delta_t)
{
  if (ImGui::Begin("Plugin Window"))
  {
    ImGui::Text("Hello from your C++ plugin!");
  }
  ImGui::End();

  // Draw the boids
  draw_boids();
}

//----------------------------------------------------------------------------//
static void on_build_debug_view(io_float32_t delta_t)
{
  ImGui::Text("Hello from by debug view!");
}

//----------------------------------------------------------------------------//
static void on_tick(io_float32_t delta_t)
{
  // Draw some text
  io_ui->draw_text("Hello from your C++ plugin!", io_cvt(glm::vec2(0.5f)),
                   io_cvt(glm::vec2(0.0f)), io_cvt(glm::vec4(1.0f)),
                   io_ui_text_alignment_center_vertical_and_horizontal);

  // Add frame delta time to our accumulator
  io_accumulator_add(delta_t, &fixed_accum);

  // Simulate our boids using a fixed step of 30 Hz
  while (io_accumulator_step(&fixed_accum))
  {
    simulate_boids();
  }

  // Draw the boids
  draw_boids();
}

//----------------------------------------------------------------------------//
IO_API_EXPORT io_uint32_t IO_API_CALL get_api_version()
{
  return IO_API_VERSION;
}

//----------------------------------------------------------------------------//
IO_API_EXPORT io_int32_t IO_API_CALL load_plugin(void* api_manager)
{
  io_api_manager = (io_api_manager_i*)api_manager;

  // Retrieve API interfaces
  io_logging =
      (const io_logging_i*)io_api_manager->find_first(IO_LOGGING_API_NAME);
  io_base = (const io_base_i*)io_api_manager->find_first(IO_BASE_API_NAME);
  io_ui = (const io_ui_i*)io_api_manager->find_first(IO_UI_API_NAME);
  io_entity =
      (const io_entity_i*)io_api_manager->find_first(IO_ENTITY_API_NAME);
  io_component_node = (const io_component_node_i*)io_api_manager->find_first(
      IO_COMPONENT_NODE_API_NAME);
  io_input_system = (const io_input_system_i*)io_api_manager->find_first(
      IO_INPUT_SYSTEM_API_NAME);
  io_editor =
      (const io_editor_i*)io_api_manager->find_first(IO_EDITOR_API_NAME);
  io_custom_components =
      (const io_custom_components_i*)io_api_manager->find_first(
          IO_CUSTOM_COMPONENTS_API_NAME);
  io_component_voxel_shape =
      (const io_component_voxel_shape_i*)io_api_manager->find_first(
          IO_COMPONENT_VOXEL_SHAPE_API_NAME);
  io_resource_palette =
      (const io_resource_palette_i*)io_api_manager->find_first(
          IO_RESOURCE_PALETTE_API_NAME);
  io_debug_geometry = (const io_debug_geometry_i*)io_api_manager->find_first(
      IO_DEBUG_GEOMETRY_API_NAME);

  // Create our boid custom component
  boid_component_mgr = io_custom_components->request_manager();
  {
    io_custom_components->register_property(
        boid_component_mgr, "Position",
        io_base->variant_from_vec3(io_cvt(glm::vec3(0.0f))),
        (void**)&comp_boid_position, 0);
    io_custom_components->register_property(
        boid_component_mgr, "PrevPosition",
        io_base->variant_from_vec3(io_cvt(glm::vec3(0.0f))),
        (void**)&comp_boid_prev_position, 0);
    io_custom_components->register_property(
        boid_component_mgr, "Velocity",
        io_base->variant_from_vec3(io_cvt(glm::vec3(0.0f))),
        (void**)&comp_boid_velocity, 0);
  }
  io_custom_components->init_manager(boid_component_mgr, "Boid");

  // Initialize the fixed time step accumulator. Use a slow tick
  // interval to showcase the interpolation of the visuals
  io_init(5.0f, &fixed_accum);

  // Set up and register our task API
  io_user_task = {};
  io_user_task.on_tick = on_tick;
  io_api_manager->register_api(IO_USER_TASK_API_NAME, &io_user_task);

  // Set up and register our editor API
  io_user_editor = {};
  io_user_editor.on_build_plugin_menu = on_build_plugin_menu;
  io_user_editor.on_tick = on_editor_tick;
  io_api_manager->register_api(IO_USER_EDITOR_API_NAME, &io_user_editor);

  // Set up and register a custom debug view
  io_user_debug_view = {};
  io_user_debug_view.on_build_debug_view = on_build_debug_view;
  io_api_manager->register_api(IO_USER_DEBUG_VIEW_API_NAME,
                               &io_user_debug_view);

  // Set up Dear ImGui
  {
    auto ctxt = (ImGuiContext*)io_base->imgui_get_context();
    ImGui::SetCurrentContext(ctxt);

    ImGuiMemAllocFunc alloc_func;
    ImGuiMemFreeFunc free_func;
    io_base->imgui_get_allocator_functions((void**)&alloc_func,
                                           (void**)&free_func);
    ImGui::SetAllocatorFunctions(alloc_func, free_func);
  }

  return 0;
}

//----------------------------------------------------------------------------//
IO_API_EXPORT void IO_API_CALL unload_plugin()
{
  // Release our custom component manager
  io_custom_components->release_and_destroy_manager(boid_component_mgr);

  // Release our user interfaces
  io_api_manager->unregister_api(&io_user_task);
  io_api_manager->unregister_api(&io_user_editor);
  io_api_manager->unregister_api(&io_user_debug_view);
}