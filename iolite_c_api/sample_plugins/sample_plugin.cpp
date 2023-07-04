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
const io_api_manager_i* io_api_manager;
const io_logging_i* io_logging;
const io_base_i* io_base;
const io_entity_i* io_entity;
const io_ui_i* io_ui;
const io_input_system_i* io_input_system;
const io_component_node_i* io_component_node;
const io_component_voxel_shape_i* io_component_voxel_shape;
const io_editor_i* io_editor;
const io_custom_components_i* io_custom_components;
const io_resource_palette_i* io_resource_palette;

// Interfaces we provide
//----------------------------------------------------------------------------//
io_user_task_i io_user_task = {};
io_user_editor_i io_user_editor = {};
io_user_debug_view_i io_user_debug_view = {};

// Globals
//----------------------------------------------------------------------------//
io_handle16_t test_component_mgr = {};

//----------------------------------------------------------------------------//
glm::quat* comp_accessor_orient = nullptr;

//----------------------------------------------------------------------------//
std::vector<io_ref_t> nodes;
std::vector<io_vec3_t> initial_positions;
std::vector<io_float32_t> rand_offsets;

//----------------------------------------------------------------------------//
void on_build_plugin_window()
{
  // Add a new menu item to the menu
  if (ImGui::MenuItem("Spawn Voxel Cubes"))
  {
    constexpr io_uint32_t numNodes = 2000u;

    // Pre-allocate memory
    nodes.resize(numNodes);
    initial_positions.resize(numNodes);
    rand_offsets.resize(numNodes);

    // Create a new palette and set a single blueish color
    auto palette = io_resource_palette->base.create("my_palette");
    io_resource_palette->set_color(palette, 1u, {0.5f, 0.5f, 1.0f, 1.0f});
    io_resource_palette->set_material_parameters(palette, 1u,
                                                 {1.0f, 0.0f, 0.0f, 0.0f});
    io_resource_palette->base.commit_changes(palette);

    // Spawn 2k voxel cubes
    for (uint32_t i = 0u; i < numNodes; ++i)
    {
      // Create a new node (and entity)
      auto node = io_component_node->create("cube");

      // Generate and set a random position for our cube
      const auto initial_position = io_cvt(glm::ballRand(60.0f));
      io_component_node->set_position(node, initial_position);
      io_component_node->update_transforms(node);

      // Create a new voxel shape component and attach it to the entity of our
      // node
      auto entity = io_component_node->base.get_entity(node);
      auto shape = io_component_voxel_shape->base.create(entity);

      // Set our shape to a cube voxel asset
      io_component_voxel_shape->base.set_property(
          shape, "VoxelShapeName", io_base->variant_from_string("cube_1m"));
      // Set our custom palette
      io_component_voxel_shape->base.set_property(
          shape, "PaletteName", io_base->variant_from_string("my_palette"));
      io_component_voxel_shape->base.commit_changes(shape);

      // Store metadata so we can access it later on
      initial_positions[i] = initial_position;
      nodes[i] = node;
      rand_offsets[i] = glm::linearRand(0.0f, 1.0f) * glm::two_pi<float>();
    }
  }
}

//----------------------------------------------------------------------------//
void on_editor_tick(float delta_t)
{
  static float time_passed = 0.0f;
  time_passed += delta_t;

  // Apply some random movement to our spawned cubes
  for (io_uint32_t i = 0u; i < nodes.size(); ++i)
  {
    const glm::vec3 offset = glm::vec3(
        0.0f, glm::sin(time_passed * glm::two_pi<float>() + rand_offsets[i]),
        glm::cos(time_passed * glm::two_pi<float>() + rand_offsets[i]));
    io_component_node->set_position(
        nodes[i], io_cvt(io_cvt(initial_positions[i]) + offset));
  }
  // Efficiently update all transforms in a batch
  io_component_node->update_transforms_jobified(nodes.data(), nodes.size());

  // Update the orientation of all our custom components
  const uint32_t numComps =
      io_custom_components->get_num_active_components(test_component_mgr);
  for (uint32_t i = 0u; i < numComps; ++i)
    comp_accessor_orient[i] =
        glm::quat(glm::vec3(0.0f, time_passed * glm::two_pi<float>(), 0.0f));
}

//----------------------------------------------------------------------------//
void on_build_debug_view(float delta_t)
{
  ImGui::Text("Hello from by debug view!");
}

//----------------------------------------------------------------------------//
void on_tick(float delta_t)
{
  static float time_passed = 0.0f;
  time_passed += delta_t;

  if (ImGui::Begin("Plugin Window"))
  {
    ImGui::Text("Hello from your C++ plugin!");
  }
  ImGui::End();

  // Draw some text
  io_ui->draw_text("Hello from your C++ plugin!", io_cvt(glm::vec2(0.5f)),
                   io_cvt(glm::vec2(0.0f)), io_cvt(glm::vec4(1.0f)),
                   io_ui_text_alignment_center_vertical_and_horizontal);

  // Draw rotating logo
  io_ui->push_rotation(time_passed * glm::two_pi<float>());
  io_ui->draw_image("splash", io_cvt(glm::vec2(0.5f)),
                    io_cvt(glm::vec2(0.5f, -1.0f)), io_cvt(glm::vec4(1.0f)),
                    io_cvt(glm::vec2(0.5f)));
  io_ui->pop_rotation();
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

  // Create a custom component
  test_component_mgr = io_custom_components->request_manager();
  {
    io_custom_components->register_property(
        test_component_mgr, "Orientation",
        io_base->variant_from_quat(io_cvt(glm::quat(1.0f, 0.0f, 0.0f, 0.0f))),
        (void**)&comp_accessor_orient, io_property_flags_runtime_only);
    io_custom_components->register_property(
        test_component_mgr, "String", io_base->variant_from_string("Hello!"),
        nullptr, 0u);
  }
  io_custom_components->init_manager(test_component_mgr, "MyCustomComponent");

  // Set up and register our task API
  io_user_task = {};
  io_user_task.on_tick = on_tick;
  io_api_manager->register_api(IO_USER_TASK_API_NAME, &io_user_task);

  // Set up and register our editor API
  io_user_editor = {};
  io_user_editor.on_build_plugin_menu = on_build_plugin_window;
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
  io_custom_components->release_and_destroy_manager(test_component_mgr);

  // Release our user interfaces
  io_api_manager->unregister_api(&io_user_task);
  io_api_manager->unregister_api(&io_user_editor);
  io_api_manager->unregister_api(&io_user_debug_view);
}