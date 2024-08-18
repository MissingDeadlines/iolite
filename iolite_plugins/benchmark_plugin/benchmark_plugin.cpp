// MIT License
//
// Copyright (c) 2024 Missing Deadlines (Benjamin Wrensch)
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
#include <vector>
#include <gtx/norm.hpp>
#include "imgui.h"
#include "IconsFontAwesome6.h"

// API
#define IO_USER_VEC3_TYPE glm::vec3
#define IO_USER_QUAT_TYPE glm::quat
#include "iolite_api.h"
#define _IO_PLUGIN_NAME "Benchmark"
#include "iolite_plugins_common.h"

// Interfaces we use
//----------------------------------------------------------------------------//
static const io_api_manager_i* io_api_manager = nullptr;
static const io_base_i* io_base = nullptr;
static const io_ui_i* io_ui = nullptr;
static const io_settings_i* io_settings = nullptr;
static const io_application_i* io_application = nullptr;
static const io_world_i* io_world = nullptr;
static const io_editor_i* io_editor = nullptr;
static const io_logging_i* io_logging = nullptr;
static const io_entity_i* io_entity = nullptr;
static const io_component_node_i* io_component_node = nullptr;
static const io_component_camera_i* io_component_camera = nullptr;
static const io_component_tag_i* io_component_tag = nullptr;
static const io_component_track_i* io_component_track = nullptr;
static const io_low_level_imgui_i* io_low_level_imgui = nullptr;
static io_user_editor_i io_user_editor = {};
static io_user_task_i io_user_task = {};

//----------------------------------------------------------------------------//
namespace benchmark
{

static struct settings_t
{
  io_uint32_t vsync;
  io_uint32_t refresh_rate_limit;

} previous_settings;

//----------------------------------------------------------------------------//
struct look_at_point_t
{
  io_vec3_t p;
  io_float32_t d;
};

//----------------------------------------------------------------------------//
struct section_t
{
  io_ref_t track{};
  io_float32_t distance{0.0f};
  io_float32_t time_passed{0.0f};
  std::vector<look_at_point_t> look_at_points;
  std::vector<float> benchmark_result;
};

//----------------------------------------------------------------------------//
static std::vector<section_t> active_sections;
static std::vector<section_t> finished_sections;

//----------------------------------------------------------------------------//
constexpr io_float32_t speed_in_mps = 5.0f;
constexpr io_float32_t fade_in_out_duration_in_s = 2.0f;
constexpr io_float32_t pause_between_sections_in_s = 1.0f;
constexpr io_float32_t look_at_rigidness = 1.0f;
constexpr io_float32_t look_at_distance_in_m = 1000.0f;
constexpr io_float32_t look_at_blend_distance_in_m = 5.0f;

//----------------------------------------------------------------------------//
static void start(const std::vector<section_t>& sections)
{
  active_sections = sections;
  finished_sections.clear();

  common::log_message(io_logging, "Starting IOLITE benchmark...");
  common::log_message(io_logging, "---");

  // Remember current and update settings
  previous_settings.refresh_rate_limit =
      io_settings->get_uint("refresh_rate_limit");
  io_settings->set_uint("refresh_rate_limit", 0);
  previous_settings.vsync = io_settings->get_uint("vsync");
  io_settings->set_uint("vsync", 3);

  // Switch to game mode
  io_editor->set_enabled(false);
}

//----------------------------------------------------------------------------//
static void stop_internal()
{
  common::log_message(io_logging, "IOLITE benchmark completed!");
  common::log_message(io_logging, "---");

  std::vector<float> section_results;

  for (const auto& s : finished_sections)
  {
    float mean_fps = 0.0f;
    for (float fps : s.benchmark_result)
      mean_fps += fps;
    mean_fps /= s.benchmark_result.size();

    section_results.emplace_back(mean_fps);
  }

  common::log_message(io_logging, "Section results:");
  common::log_message(io_logging, "---");

  float result = 0.0f;
  uint32_t section_id = 1u;
  for (float r : section_results)
  {
    common::log_message(io_logging, "  Section %u: %.3f FPS", section_id, r);
    result += 1.0f / r;
    ++section_id;
  }
  result = section_results.size() / result;

  common::log_message(io_logging, "---");
  common::log_message(io_logging, "Final score: %u points",
                      (uint32_t)std::round(result * 1000.0f));
  common::log_message(io_logging, "---");

  // Restore settings
  io_settings->set_uint("refresh_rate_limit",
                        previous_settings.refresh_rate_limit);
  io_settings->set_uint("vsync", previous_settings.vsync);

  // Enable the editor again
  io_editor->set_enabled(true);
}

//----------------------------------------------------------------------------//
static void cache_look_at_points(io_ref_t track,
                                 std::vector<look_at_point_t> look_at_points)
{
  // Cache look-at points
  if (look_at_distance_in_m > glm::epsilon<float>())
  {
    io_size_t entities_length;
    io_component_tag->find_entities_with_tag("look_at", nullptr,
                                             &entities_length);
    std::vector<io_ref_t> look_at_point_nodes(entities_length);
    io_component_tag->find_entities_with_tag(
        "look_at", look_at_point_nodes.data(), &entities_length);

    const float look_at_distance2 =
        look_at_distance_in_m * look_at_distance_in_m;
    for (const auto entity : look_at_point_nodes)
    {
      const auto node =
          io_component_node->base.get_component_for_entity(entity);
      const auto look_at_pos = io_component_node->get_world_position(node);

      float dist_to_closest_point;
      const auto closest_point =
          io_component_track->find_closest_point_on_track(
              track, look_at_pos, &dist_to_closest_point, 0.1f);

      if (glm::length2(io_cvt(closest_point) - io_cvt(look_at_pos)) <
          look_at_distance2)
      {
        // Sort by distance
        const auto it = std::upper_bound(
            look_at_points.begin(), look_at_points.end(), dist_to_closest_point,
            [](float value, const look_at_point_t& candidate) {
              return value < candidate.d;
            });

        look_at_points.insert(it, {
                                      look_at_pos,
                                      dist_to_closest_point,
                                  });
      }
    }
  }
}

//----------------------------------------------------------------------------//
static void stop() { active_sections.clear(); }

//----------------------------------------------------------------------------//
static auto is_active() { return !active_sections.empty(); }

//----------------------------------------------------------------------------//
static glm::quat damp(const glm::quat& r0, const glm::quat& r1, float damping,
                      float dt)
{
  return glm::slerp(r0, r1, 1.0f - glm::exp(-dt * damping));
}

//----------------------------------------------------------------------------//
static void update(float dt)
{
  if (active_sections.empty())
    return;

  auto& current_section = active_sections.front();

  const auto cam = io_world->get_active_camera();
  const auto cam_entity = io_component_camera->base.get_entity(cam);
  const auto cam_node =
      io_component_node->base.get_component_for_entity(cam_entity);

  // Initialize camera orientation
  if (current_section.distance <= glm::epsilon<float>())
  {
    io_vec3_t pos;
    io_component_track->calc_world_position_on_track_absolute(
        current_section.track, 0.0f, &pos);
    io_vec3_t look_at_pos;
    io_component_track->calc_world_position_on_track_absolute(
        current_section.track, 1.0f, &look_at_pos);

    const auto cam_dir = glm::normalize(io_cvt(look_at_pos) - io_cvt(pos));
    const auto target_orient =
        glm::quatLookAt(-cam_dir, glm::vec3(0.0f, 1.0f, 0.0f));

    io_component_node->set_orientation(cam_node, io_cvt(target_orient));
  }

  const float track_length =
      io_component_track->get_length(current_section.track);

  // Fade from/to black based on track position
  {
    constexpr float fade_in_out_time = 2.0f;
    const float fade_distance = speed_in_mps * fade_in_out_time;

    const float fade_in =
        glm::min(current_section.distance / fade_distance, 1.0f);
    const float fade_out =
        glm::min(1.0f - glm::max(current_section.distance -
                                     (track_length - fade_distance),
                                 0.0f) /
                            fade_distance,
                 1.0f);

    float fade = glm::min(fade_in, fade_out);
    fade = fade * fade * fade;

    io_ui->push_style_var_float(io_ui_style_var_alpha, 1.0f - fade);
    io_ui->draw_rect({0.0f, 0.0f, 0.0f, 1.0f});
    io_ui->pop_style_var();
  }

  current_section.time_passed += dt;

  if (current_section.time_passed < pause_between_sections_in_s)
    return;

  current_section.distance += dt * speed_in_mps;

  if (current_section.distance < track_length)
  {
    io_vec3_t pos;
    io_component_track->calc_world_position_on_track_absolute(
        current_section.track, current_section.distance, &pos);

    glm::vec3 cam_dir;

    io_vec3_t look_at_pos;
    io_component_track->calc_world_position_on_track_absolute(
        current_section.track, current_section.distance + 1.0f, &look_at_pos);

    const uint32_t num_look_at_points = current_section.look_at_points.size();
    if (num_look_at_points > 0u)
    {
      auto idx = (uint32_t)-1;
      for (uint32_t i = 0u; i < num_look_at_points; ++i)
      {
        const auto& lp0 = current_section.look_at_points[i];
        if (i >= num_look_at_points - 1u ||
            current_section.look_at_points[i + 1u].d > current_section.distance)
        {
          idx = i;
          break;
        }
      }

      if (idx >= num_look_at_points - 1u ||
          look_at_blend_distance_in_m <= glm::epsilon<float>())
      {
        look_at_pos = current_section.look_at_points[idx].p;
      }
      else
      {
        const auto& lp0 = current_section.look_at_points[idx];
        const auto& lp1 = current_section.look_at_points[idx + 1u];

        const float look_at_distance =
            glm::min(lp1.d - lp0.d, look_at_blend_distance_in_m);
        look_at_pos =
            io_cvt(glm::mix(io_cvt(lp0.p), io_cvt(lp1.p),
                            1.0f - glm::min((lp1.d - current_section.distance) /
                                                look_at_distance,
                                            1.0f)));
      }
    }

    cam_dir = glm::normalize(io_cvt(look_at_pos) - io_cvt(pos));

    const auto current_orient = io_component_node->get_orientation(cam_node);
    const auto target_orient =
        glm::quatLookAt(-cam_dir, glm::vec3(0.0f, 1.0f, 0.0f));

    const auto smoothed_cam_orient =
        damp(io_cvt(current_orient), target_orient, look_at_rigidness, dt);

    io_component_node->set_orientation(cam_node, io_cvt(smoothed_cam_orient));
    io_component_node->set_position(cam_node, pos);
    io_component_node->update_transforms(cam_node);

    // Store the result for this frame
    current_section.benchmark_result.emplace_back(io_application->get_fps());
  }
  else
  {
    // Advance to the next section
    finished_sections.push_back({});
    finished_sections.back() = std::move(active_sections.front());
    active_sections.erase(active_sections.begin());
  }

  if (active_sections.empty())
    stop_internal();
}
} // namespace benchmark

//----------------------------------------------------------------------------//
static void on_build_plugin_menu()
{
  if (ImGui::MenuItem(ICON_FA_ROCKET_LAUNCH "   Run Benchmark"))
  {
    // Collect sections for this benchmark
    std::vector<benchmark::section_t> sections;

    const uint32_t num_tracks =
        io_component_track->base.get_num_active_components();
    const auto track_entities = io_component_track->base.get_entity_memory();
    for (uint32_t i = 0u; i < num_tracks; ++i)
    {
      const auto track_entity = track_entities[i];
      const auto name = io_entity->get_name(track_entity);

      if (strstr(name, "benchmark") != nullptr)
      {
        benchmark::section_t section = {};
        {
          section.track =
              io_component_track->base.get_component_for_entity(track_entity);
          benchmark::cache_look_at_points(section.track,
                                          section.look_at_points);
        }
        sections.emplace_back(section);
      }
    }

    if (!sections.empty())
      benchmark::start(sections);
  }
}

//----------------------------------------------------------------------------//
static io_bool_t on_build_node_action_menu(io_ref_t node, const io_ref_t* nodes,
                                           io_size_t num_nodes)
{
  auto entity = io_component_node->base.get_entity(node);
  auto track = io_component_track->base.get_component_for_entity(entity);

  if (io_ref_is_valid(track))
  {
    if (ImGui::BeginMenu(ICON_FA_ROCKET "   Benchmark"))
    {
      if (ImGui::MenuItem(ICON_FA_ROCKET_LAUNCH "   Benchmark Selected Tracks"))
      {
        std::vector<benchmark::section_t> sections;

        for (uint32_t i = 0u; i < num_nodes; ++i)
        {
          node = nodes[i];
          entity = io_component_node->base.get_entity(node);
          track = io_component_track->base.get_component_for_entity(entity);

          if (io_ref_is_valid(track))
          {
            benchmark::section_t section = {};
            {
              section.track = track;
              benchmark::cache_look_at_points(section.track,
                                              section.look_at_points);
            }
            sections.push_back(section);
          }
        }

        if (!sections.empty())
          benchmark::start(sections);
      }

      ImGui::EndMenu();
    }

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------//
static void on_tick(float dt) { benchmark::update(dt); }

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
    io_ui = (const io_ui_i*)io_api_manager->find_first(IO_UI_API_NAME);
    io_application = (const io_application_i*)io_api_manager->find_first(
        IO_APPLICATION_API_NAME);
    io_settings =
        (const io_settings_i*)io_api_manager->find_first(IO_SETTINGS_API_NAME);
    io_world = (const io_world_i*)io_api_manager->find_first(IO_WORLD_API_NAME);
    io_entity =
        (const io_entity_i*)io_api_manager->find_first(IO_ENTITY_API_NAME);
    io_editor =
        (const io_editor_i*)io_api_manager->find_first(IO_EDITOR_API_NAME);
    io_logging =
        (const io_logging_i*)io_api_manager->find_first(IO_LOGGING_API_NAME);
    io_component_node = (const io_component_node_i*)io_api_manager->find_first(
        IO_COMPONENT_NODE_API_NAME);
    io_component_camera =
        (const io_component_camera_i*)io_api_manager->find_first(
            IO_COMPONENT_CAMERA_API_NAME);
    io_component_track =
        (const io_component_track_i*)io_api_manager->find_first(
            IO_COMPONENT_TRACK_API_NAME);
    io_component_tag = (const io_component_tag_i*)io_api_manager->find_first(
        IO_COMPONENT_TAG_API_NAME);
    io_low_level_imgui =
        (const io_low_level_imgui_i*)io_api_manager->find_first(
            IO_LOW_LEVEL_IMGUI_API_NAME);
  }

  // Register the interfaces we provide
  io_user_editor = {};
  {
    io_user_editor.on_build_plugin_menu = on_build_plugin_menu;
    io_user_editor.on_build_node_action_menu = on_build_node_action_menu;
  }
  io_api_manager->register_api(IO_USER_EDITOR_API_NAME, &io_user_editor);

  io_user_task = {};
  {
    io_user_task.on_tick = on_tick;
  }
  io_api_manager->register_api(IO_USER_TASK_API_NAME, &io_user_task);

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
  io_api_manager->unregister_api(&io_user_editor);
  io_api_manager->unregister_api(&io_user_task);
}
