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
static const io_api_manager_i* io_api_manager = nullptr;
static const io_logging_i* io_logging = nullptr;
static const io_base_i* io_base = nullptr;
static const io_entity_i* io_entity = nullptr;
static const io_ui_i* io_ui = nullptr;
static const io_input_system_i* io_input_system = nullptr;
static const io_component_node_i* io_component_node = nullptr;
static const io_component_voxel_shape_i* io_component_voxel_shape = nullptr;
static const io_editor_i* io_editor = nullptr;
static const io_custom_components_i* io_custom_components = nullptr;
static const io_resource_palette_i* io_resource_palette = nullptr;
static const io_debug_geometry_i* io_debug_geometry = nullptr;
static const io_low_level_imgui_i* io_low_level_imgui = nullptr;

// Sample variables
//----------------------------------------------------------------------------//
static bool ui_sample_show = false;
static bool imgui_sample_show = false;
static float ui_sample_time_accum = 0.0f;

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
static io_fixed_step_accumulator_t fixed_accum;

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
    constexpr io_uint32_t num_nodes = 500u;

    // Create a new palette and set a single blueish color
    const io_ref_t palette = io_resource_palette->base.create("my_palette");
    io_resource_palette->set_color(palette, 1u, {0.5f, 0.5f, 1.0f, 1.0f});
    io_resource_palette->set_material_parameters(palette, 1u,
                                                 {1.0f, 0.0f, 0.0f, 0.0f});
    io_resource_palette->base.commit_changes(palette);

    // Spawn voxel cubes
    for (uint32_t i = 0u; i < num_nodes; ++i)
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
    constexpr io_uint32_t num_boids = 2000u;

    // Spawn boids
    for (uint32_t i = 0u; i < num_boids; ++i)
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

  ImGui::Separator();

  ImGui::Checkbox("Show Sample ImGui Window", &imgui_sample_show);
  if (ImGui::Checkbox("Show UI Sample", &ui_sample_show))
    ui_sample_time_accum = 0.0f;
}

//----------------------------------------------------------------------------//
static auto draw_ui_sample_main_menu_button(const char* text,
                                            io_float32_t& offset) -> bool
{
  constexpr float button_height = 50.0f;
  constexpr float spacing = 10.0f;

  bool clicked = false;

  io_ui->push_transform({0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, offset},
                        {0.0f, offset + button_height}, 0.0f);
  {
    // Draw button background
    {
      constexpr io_vec4_t color_default = {0.1f, 0.1f, 0.1f, 0.5f};
      constexpr io_vec4_t color_hovered = {0.25f, 0.25f, 0.25f, 0.5f};
      constexpr io_vec4_t color_pressed = {0.5f, 0.5f, 0.5f, 0.5f};

      io_vec4_t color = color_default;

      if (io_ui->intersects(io_input_system->get_mouse_pos()))
      {
        color = color_hovered;
        if (io_input_system->get_key_state(io_input_key_mouse_left, 0) ==
            io_input_key_state_pressed)
          color = color_pressed;
        else if (io_input_system->get_key_state(io_input_key_mouse_left, 0) ==
                 io_input_key_state_clicked)
          clicked = true;
      }

      // Draw rects with rounded corners
      io_ui->push_style_var_float(io_ui_style_var_rect_rounding, 0.2f);

      // Draw filled rect
      io_ui->draw_rect(color);

      // Draw 1px wide outline
      io_ui->push_style_var_float(io_ui_style_var_draw_outline, 1.0f);
      io_ui->draw_rect({1.0f, 1.0f, 1.0f, 1.0f});
      io_ui->pop_style_var();

      io_ui->pop_style_var();
    }

    // Draw button text
    io_ui->draw_text(text, io_ui_text_align_horizontal_center,
                     io_ui_text_align_vertical_center, 0);
  }
  io_ui->pop_transform();

  offset += spacing + button_height;

  return clicked;
}

//----------------------------------------------------------------------------//
static void draw_ui_sample_main_menu(io_float32_t delta_t)
{
  // Box for our menu
  io_ui->push_transform_preset(io_ui_anchor_preset_center,
                               {-200.0f, 200.0f, -300.0f, 300.0f}, 0.0f);
  {
    // Draw background
    io_ui->draw_rect({0.1f, 0.1f, 0.1f, 0.5f});

    // Margins
    io_ui->push_transform_preset(io_ui_anchor_preset_full_rect,
                                 {25.0f, -25.0f, 25.0f, -25.0f}, 0.0f);
    {
      io_ui->push_transform({0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f},
                            {0.0f, 100.0f}, 0.0f);
      {
        io_vec2_t splash_size = io_ui->get_image_size("splash");
        const float aspect = splash_size.y / splash_size.x;
        splash_size.x = 200.0f;
        splash_size.y = splash_size.x * aspect;

        // Draw logo
        io_ui->push_transform_preset(
            io_ui_anchor_preset_center,
            {-splash_size.x * 0.5f, splash_size.x * 0.5f, -splash_size.y * 0.5f,
             splash_size.y * 0.5f},
            0.0f);
        {
          io_ui->draw_image("splash", {1.0f, 1.0f, 1.0f, 1.0f});
        }
        io_ui->pop_transform();
      }
      io_ui->pop_transform();

      float offset = 125.0f;

      draw_ui_sample_main_menu_button("NEW GAME", offset);
      draw_ui_sample_main_menu_button("SETTINGS", offset);
      if (draw_ui_sample_main_menu_button("EXIT", offset))
        ui_sample_show = false;

      offset += 25.0f;

      // Draw circle and rotating pentagon inside
      constexpr float circle_radius = 15.0f;
      io_ui->push_transform_preset(io_ui_anchor_preset_center_top,
                                   {0.0f, 0.0f, offset, offset},
                                   ui_sample_time_accum * 2.0f);
      {
        io_ui->push_transform_preset(
            io_ui_anchor_preset_center,
            {-circle_radius, circle_radius, -circle_radius, circle_radius},
            0.0f);
        {
          io_ui->push_style_var_float(io_ui_style_var_draw_outline, 1.0f);
          io_ui->draw_circle({0.6f, 0.6f, 0.6f, 1.0f});
          io_ui->pop_style_var();
          io_ui->push_style_var_float(io_ui_style_var_draw_outline, 2.0f);
          io_ui->draw_ngon({1.0f, 1.0f, 1.0f, 1.0f}, 5u);
          io_ui->pop_style_var();
        }
        io_ui->pop_transform();
      }
      io_ui->pop_transform();

      io_ui->push_transform({0.0f, 10.0f}, {1.0f, -10.0f}, {0.6f, 0.0f},
                            {0.8f, -10.0f}, 0.0f);
      {
        static const char* text_template =
            "Typewriter effect! Vivamus at nulla vel ante laoreet pellentesque "
            "eu vitae justo. Cras egestas egestas augue, vel laoreet massa "
            "bibendum id.";
        static const uint32_t text_template_length = strlen(text_template) + 1u;
        static char text[256u];

        static uint32_t letter_index = 0u;

        strncpy(text, text_template, letter_index);
        text[letter_index] = '\0';

        static float time_since_last_letter = 0.0f;
        time_since_last_letter += delta_t;

        if (time_since_last_letter > 0.1f)
        {
          letter_index = (letter_index + 1u) % text_template_length;
          // Whitespaces feel like hickups; skip them
          if (text_template[letter_index] == ' ')
            letter_index = (letter_index + 1u) % text_template_length;

          time_since_last_letter = 0.0f;
        }

        io_ui->push_style_var_vec4(io_ui_style_var_text_color,
                                   {1.0f, 0.75f, 0.75f, 1.0f});
        io_ui->push_font_size(12.0f);
        io_ui->draw_text(text, io_ui_text_align_horizontal_center,
                         io_ui_text_align_vertical_center,
                         io_ui_text_flag_wrap);
        io_ui->pop_font_size();
        io_ui->pop_style_var();
      }
      io_ui->pop_transform();

      io_ui->push_transform({0.0f, 10.0f}, {1.0f, -10.0f}, {0.8f, 0.0f},
                            {1.0f, -10.0f}, 0.0f);
      {
        io_ui->clip_children();

        io_ui->push_transform_preset(
            io_ui_anchor_preset_full_rect,
            {0.0f, 0.0f, glm::sin(ui_sample_time_accum) * 50.0f, 0.0f}, 0.0f);
        {
          io_ui->push_font_size(12.0f);
          io_ui->draw_text(
              "Here's some small text so showcase clipping and wrapping. And "
              "one "
              "or two new lines!\n\nLorem Ipsum is simply dummy text of the "
              "printing and typesetting "
              "industry. Lorem Ipsum has been the industry's standard dummy "
              "text "
              "ever since the 1500s, when an unknown printer took a galley of "
              "type and scrambled it to make a type specimen book. It has "
              "survived not only five centuries, but also the leap into "
              "electronic typesetting, remaining essentially unchanged. It was "
              "popularised in the 1960s with the release of Letraset sheets "
              "containing Lorem Ipsum passages, and more recently with desktop "
              "publishing software like Aldus PageMaker including versions of "
              "Lorem Ipsum.",
              io_ui_text_align_horizontal_left, io_ui_text_align_vertical_top,
              io_ui_text_flag_wrap);
        }
        io_ui->pop_transform();
      }
      io_ui->pop_transform();
      io_ui->pop_font_size();
    }
    io_ui->pop_transform();
  }
  io_ui->pop_transform();
}

//----------------------------------------------------------------------------//
static void draw_ui_sample(io_float32_t delta_t)
{
  static bool scale_ui = false;

  // Minimal ImGui for configuring the sample
  if (ImGui::Begin("UI Sample Settings"))
  {
    ImGui::Checkbox("Scale UI", &scale_ui);
  }
  ImGui::End();

  io_input_system->request_mouse_cursor();

  ui_sample_time_accum += delta_t;
  const float alpha = glm::min(1.0f, ui_sample_time_accum);

  // Fade UI in
  io_ui->push_style_var_float(io_ui_style_var_alpha, alpha);

  // The base canvas size we're targeting
  io_vec2_t base_canvas_size = {1920.0f, 1080.0f};

  // Opaque background spanning the whole viewport
  io_ui->draw_rect({0.0f, 0.0f, 0.0f, 1.0f});

  if (scale_ui)
  {
    // This function helps us calculating a scaling factor and offset that
    // scales the provided base canvas size to the full size of the viewport
    io_ui->push_scale_offset_for_base_size(base_canvas_size,
                                           io_ui_aspect_mode_keep);
    // Push our base canvas size as the active transform
    io_ui->push_transform({0.0f, 0.0f}, {0.0f, base_canvas_size.x},
                          {0.0f, 0.0f}, {0.0f, base_canvas_size.y}, 0.0f);
  }

  // Global margin
  io_ui->push_transform({0.0f, 30.0f}, {1.0f, -30.0f}, {0.0f, 30.0f},
                        {1.0f, -30.0f}, 0.0f);
  {
    draw_ui_sample_main_menu(delta_t);
  }

  io_ui->pop_transform();

  if (scale_ui)
  {
    io_ui->pop_transform();
    io_ui->pop_scale_offset();
  }

  io_ui->pop_style_var();
}

//----------------------------------------------------------------------------//
static void on_editor_tick(io_float32_t delta_t)
{
  if (imgui_sample_show)
  {
    if (ImGui::Begin("Plugin Window", &imgui_sample_show))
    {
      ImGui::Text("Hello from your C++ plugin!");
    }
    ImGui::End();
  }

  // Draw the boids
  draw_boids();

  // Draw the UI sample (if active)
  if (ui_sample_show)
    draw_ui_sample(delta_t);
}

//----------------------------------------------------------------------------//
static void on_build_debug_view(io_float32_t delta_t)
{
  ImGui::Text("Hello from your plugin debug view!");
}

//----------------------------------------------------------------------------//
static void on_tick(io_float32_t delta_t)
{
  // Add frame delta time to our accumulator
  io_accumulator_add(delta_t, &fixed_accum);
  // Simulate our boids using our fixed stepper
  while (io_accumulator_step(&fixed_accum))
    simulate_boids();

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
  io_api_manager = (const io_api_manager_i*)api_manager;

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
  io_low_level_imgui = (const io_low_level_imgui_i*)io_api_manager->find_first(
      IO_LOW_LEVEL_IMGUI_API_NAME);

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
  io_init_fixed_step_accumulator(&fixed_accum, 5.0f);

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
    auto ctxt = (ImGuiContext*)io_low_level_imgui->get_imgui_context();
    ImGui::SetCurrentContext(ctxt);

    ImGuiMemAllocFunc alloc_func;
    ImGuiMemFreeFunc free_func;
    io_low_level_imgui->get_imgui_allocator_functions((void**)&alloc_func,
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