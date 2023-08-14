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

#define STB_SPRINTF_IMPLEMENTATION
#include "lua_plugin.h"

// Globals
//----------------------------------------------------------------------------//
char string_buffer[string_buffer_length];

// Interfaces we use
//----------------------------------------------------------------------------//
const io_api_manager_i* io_api_manager = {};
const io_logging_i* io_logging = {};
const io_base_i* io_base = {};
const io_entity_i* io_entity = {};
const io_ui_i* io_ui = {};
const io_filesystem_i* io_filesystem = {};
const io_settings_i* io_settings = {};
const io_save_data_i* io_save_data = {};
const io_world_i* io_world = {};
const io_particle_system_i* io_particle_system = {};
const io_sound_i* io_sound = {};
const io_input_system_i* io_input_system = {};
const io_plugin_terrain_i* io_plugin_terrain = {};
const io_physics_i* io_physics = {};
const io_debug_geometry_i* io_debug_geometry = {};
const io_pathfinding_i* io_pathfinding = {};

const io_component_node_i* io_component_node = {};
const io_component_custom_data_i* io_component_custom_data = {};
const io_component_tag_i* io_component_tag = {};
const io_component_flipbook_animation_i* io_component_flipbook_animation = {};
const io_component_post_effect_volume_i* io_component_post_effect_volume = {};
const io_component_camera_i* io_component_camera = {};
const io_component_script_i* io_component_script = {};
const io_component_voxel_shape_i* io_component_voxel_shape = {};
const io_component_light_i* io_component_light = {};
const io_component_character_controller_i* io_component_character_controller =
    {};
const io_component_camera_controller_i* io_component_camera_controller = {};
const io_component_particle_i* io_component_particle = {};

// Interfaces we provide
//----------------------------------------------------------------------------//
io_user_script_i io_user_script = {};
io_user_events_i io_user_events = {};

// Macros
//----------------------------------------------------------------------------//
#define SOL_VALIDATE_RESULT(_func, _scriptName)                                \
  {                                                                            \
    auto result = _func;                                                       \
    if (!result.valid())                                                       \
    {                                                                          \
      sol::error err = result;                                                 \
      stbsp_snprintf(string_buffer, string_buffer_length,                      \
                     "Lua script '%s' failed: %s", _scriptName, err.what());   \
      io_logging->log_info(string_buffer);                                     \
    }                                                                          \
  }

//----------------------------------------------------------------------------//
struct script_instance_t
{
  const char* script_name;
  io_ref_t entity;
  sol::state* state;
};
static std::vector<script_instance_t> script_instances;

//----------------------------------------------------------------------------//
static void register_script_instance(const char* script_name, io_ref_t entity,
                                     sol::state* state)
{
  script_instances.push_back({script_name, entity, state});
}

//----------------------------------------------------------------------------//
static void unregister_script_instance(sol::state* state)
{
  for (auto it = script_instances.begin(); it != script_instances.end();)
  {
    if (it->state == state)
      it = script_instances.erase(it);
    else
      ++it;
  }
}

//----------------------------------------------------------------------------//
namespace internal
{
//----------------------------------------------------------------------------//
static std::vector<std::string> queued_world_loads;
static std::vector<io_ref_t> queued_nodes_to_destroy;

//----------------------------------------------------------------------------//
static bool scripts_active = false;

//----------------------------------------------------------------------------//
void queue_load_world(const char* world_name)
{
  queued_world_loads.push_back(world_name);
}

//----------------------------------------------------------------------------//
void queue_destroy_node(io_ref_t node)
{
  queued_nodes_to_destroy.push_back(node);
}

//----------------------------------------------------------------------------//
void execute_queued_actions()
{
  for (auto node : queued_nodes_to_destroy)
  {
    if (io_base->ref_is_valid(node) && io_component_node->base.is_alive(node))
      io_component_node->base.destroy(node);
  }
  queued_nodes_to_destroy.clear();

  // Clear upfront to avoid recursions
  auto temp_worlds_to_load = queued_world_loads;
  queued_world_loads.clear();
  for (const auto& world_name : temp_worlds_to_load)
    io_world->load_world(world_name.c_str());
}

//----------------------------------------------------------------------------//
auto load_script(sol::state& state, const char* filepath)
    -> sol::protected_function
{

  uint32_t buffer_length;
  if (io_filesystem->load_file_from_data_source(filepath, nullptr,
                                                &buffer_length))
  {
    std::vector<uint8_t> data(buffer_length);
    io_filesystem->load_file_from_data_source(filepath, data.data(),
                                              &buffer_length);

    sol::load_result script =
        state.load_buffer((const char*)data.data(), data.size());
    if (!script.valid())
    {
      const sol::error err = script;
      stbsp_snprintf(string_buffer, string_buffer_length,
                     "Lua script '%s' failed to load: %s", filepath,
                     err.what());
      io_logging->log_info(string_buffer);
    }
    else
    {
      stbsp_snprintf(string_buffer, string_buffer_length,
                     "Lua script '%s' loaded...", filepath);
      io_logging->log_info(string_buffer);

      return script;
    }
  }

  return sol::protected_function();
}

//----------------------------------------------------------------------------//
void execute_script(sol::state& state, const char* directory_path,
                    const char* script_name)
{
  const std::string filepath =
      std::string(directory_path) + "/" + script_name + ".lua";

  auto script = load_script(state, filepath.c_str());
  if (script.valid() && script().valid())
    state["__ScriptName"] = script_name;
}

//----------------------------------------------------------------------------//
void script_activate(sol::state& state, io_ref_t entity,
                     uint32_t update_interval)
{
  if (!scripts_active || state["__ScriptIsActive"].get<bool>())
    return;

  sol::protected_function on_activate = state["OnActivate"];
  if (on_activate.valid())
  {
    SOL_VALIDATE_RESULT(on_activate(entity),
                        state["__ScriptName"].get<const char*>());
  }

  state["__ScriptIsActive"] = true;

  // Ensure that not all components get updated in the same frame
  state["__TimeSinceLastUpdate"] =
      glm::linearRand(0.0f, update_interval / 1000.0f);
}

//----------------------------------------------------------------------------//
void script_deactivate(sol::state& state, io_ref_t entity)
{
  if (!scripts_active || !state["__ScriptIsActive"].get<bool>())
    return;

  sol::protected_function on_deactivate = state["OnDeactivate"];
  if (on_deactivate.valid())
  {
    SOL_VALIDATE_RESULT(on_deactivate(entity),
                        state["__ScriptName"].get<const char*>());
  }

  state["__ScriptIsActive"] = false;
}

//----------------------------------------------------------------------------//
void script_tick(sol::state& state, io_float32_t delta_t, io_ref_t entity)
{
  if (!scripts_active || !state["__ScriptIsActive"].get<bool>())
    return;

  sol::protected_function tick = state["Tick"];
  if (tick.valid())
    SOL_VALIDATE_RESULT(tick(entity, delta_t),
                        state["__ScriptName"].get<const char*>());
}

// TODO
//----------------------------------------------------------------------------//
void script_tick_async(sol::state& state, io_float32_t delta_t, io_ref_t entity)
{
  if (!scripts_active || !state["__ScriptIsActive"].get<bool>())
    return;

  sol::protected_function tick = state["TickAsync"];
  if (tick.valid())
    SOL_VALIDATE_RESULT(tick(entity, delta_t),
                        state["__ScriptName"].get<const char*>());
}

//----------------------------------------------------------------------------//
void script_update(sol::state& state, io_float32_t delta_t, io_ref_t entity,
                   uint32_t update_interval)
{
  if (!scripts_active || !state["__ScriptIsActive"].get<bool>())
    return;

  const float update_interval_in_s = update_interval / 1000.0f;
  float time_since_last_update =
      state["__TimeSinceLastUpdate"].get<float>() + delta_t;

  while (time_since_last_update >= update_interval_in_s)
  {
    sol::protected_function update = state["Update"];
    if (update.valid())
      SOL_VALIDATE_RESULT(update(entity, update_interval_in_s),
                          state["__ScriptName"].get<const char*>());

    time_since_last_update -= update_interval_in_s;
  }

  state["__TimeSinceLastUpdate"] = time_since_last_update;
}

} // namespace internal

//----------------------------------------------------------------------------//
static void on_init_script(const char* script_name, io_ref_t entity,
                           io_uint32_t update_interval, void** user_data)
{
  auto** state = (sol::state**)user_data;

  void* mem = io_base->mem_allocate(sizeof(sol::state));
  *state = new (mem) sol::state();

  register_script_instance(script_name, entity, *state);

  internal::script_init_state(**state);

  if (strlen(script_name) > 0)
    internal::execute_script(**state, "media/scripts", script_name);

  internal::script_activate(**state, entity, update_interval);
  internal::execute_queued_actions();
}

//----------------------------------------------------------------------------//
static void on_destroy_script(io_ref_t entity, void** user_data)
{
  auto** s = (sol::state**)user_data;

  internal::script_deactivate(**s, entity);

  internal::execute_queued_actions();
  unregister_script_instance(*s);

  (*s)->~state();
  io_base->mem_free(*s);
  *s = nullptr;
}

//----------------------------------------------------------------------------//
static void on_activate_scripts(const io_user_script_batch_t* scripts,
                                io_uint32_t scripts_length)
{
  internal::scripts_active = true;

  for (uint32_t i = 0u; i < scripts_length; ++i)
  {
    auto* state = (sol::state*)scripts->user_datas[i];
    internal::script_activate(*state, scripts->entities[i],
                              scripts->update_intervals[i]);
  }

  internal::execute_queued_actions();
}

//----------------------------------------------------------------------------//
static void on_deactivate_scripts(const io_user_script_batch_t* scripts,
                                  io_uint32_t scripts_length)
{
  for (uint32_t i = 0u; i < scripts_length; ++i)
  {
    auto* state = (sol::state*)scripts->user_datas[i];
    internal::script_deactivate(*state, scripts->entities[i]);
  }

  internal::execute_queued_actions();
  internal::scripts_active = false;
}

//----------------------------------------------------------------------------//
static void on_tick_scripts(io_float32_t delta_t,
                            const io_user_script_batch_t* scripts,
                            io_uint32_t scripts_length)
{
  for (uint32_t i = 0u; i < scripts_length; ++i)
  {
    auto* state = (sol::state*)scripts->user_datas[i];
    internal::script_tick(*state, delta_t, scripts->entities[i]);
    // TODO
    internal::script_tick_async(*state, delta_t, scripts->entities[i]);
    internal::script_update(*state, delta_t, scripts->entities[i],
                            scripts->update_intervals[i]);
  }

  internal::execute_queued_actions();
}

//----------------------------------------------------------------------------//
static void on_script_changed(const char* filename)
{
  const auto filename_without_extension =
      std::filesystem::path(filename).filename().replace_extension().u8string();

  for (auto& script : script_instances)
  {
    if (strcmp(script.script_name,
               (const char*)filename_without_extension.c_str()) == 0)
    {
      internal::execute_script(*script.state, "media/scripts",
                               script.script_name);
    }
  }
}

//----------------------------------------------------------------------------//
static void on_physics_events(const io_events_header_t* begin,
                              const io_events_header_t* end)
{
  if (!internal::scripts_active)
    return;

  std::vector<lua_physics_contact_event_t> lua_events;
  lua_events.reserve(64u);

  const io_events_header_t* event = begin;
  const io_name_t contact_event_type = io_to_name("physics_contact");

  // Collect events
  while (event < end)
  {
    if (event->type.hash == contact_event_type.hash)
    {
      auto* contact =
          (io_events_data_physics_contact_t*)io_events_get_data(event);

      lua_events.push_back({"physics_contact",
                            {contact->entity0, contact->entity1, contact->pos,
                             contact->impulse}});
    }

    event = io_events_get_next(event, end);
  }

  for (auto& script : script_instances)
  {
    const auto& s = *script.state;
    if (!s["__ScriptIsActive"].get<bool>())
      return;

    sol::protected_function on_event = s["OnEvent"];
    if (on_event.valid())
      SOL_VALIDATE_RESULT(on_event(script.entity, lua_events),
                          s["__ScriptName"].get<const char*>());
  }
}

//----------------------------------------------------------------------------//
IO_API_EXPORT io_uint32_t IO_API_CALL get_api_version()
{
  return IO_API_VERSION;
}

//----------------------------------------------------------------------------//
IO_API_EXPORT int IO_API_CALL load_plugin(void* api_manager)
{
  io_api_manager = (const io_api_manager_i*)api_manager;

  // Core intefaces
  io_logging =
      (const io_logging_i*)io_api_manager->find_first(IO_LOGGING_API_NAME);
  io_base = (const io_base_i*)io_api_manager->find_first(IO_BASE_API_NAME);
  io_ui = (const io_ui_i*)io_api_manager->find_first(IO_UI_API_NAME);
  io_entity =
      (const io_entity_i*)io_api_manager->find_first(IO_ENTITY_API_NAME);
  io_filesystem = (const io_filesystem_i*)io_api_manager->find_first(
      IO_FILESYSTEM_API_NAME);
  io_settings =
      (const io_settings_i*)io_api_manager->find_first(IO_SETTINGS_API_NAME);
  io_save_data =
      (const io_save_data_i*)io_api_manager->find_first(IO_SAVE_DATA_API_NAME);
  io_world = (const io_world_i*)io_api_manager->find_first(IO_WORLD_API_NAME);
  io_particle_system = (const io_particle_system_i*)io_api_manager->find_first(
      IO_PARTICLE_SYSTEM_API_NAME);
  io_sound = (const io_sound_i*)io_api_manager->find_first(IO_SOUND_API_NAME);
  io_input_system = (const io_input_system_i*)io_api_manager->find_first(
      IO_INPUT_SYSTEM_API_NAME);
  io_physics =
      (const io_physics_i*)io_api_manager->find_first(IO_PHYSICS_API_NAME);
  io_debug_geometry = (const io_debug_geometry_i*)io_api_manager->find_first(
      IO_DEBUG_GEOMETRY_API_NAME);
  io_pathfinding = (const io_pathfinding_i*)io_api_manager->find_first(
      IO_PATHFINDING_API_NAME);

  // Component interfaces
  io_component_custom_data =
      (const io_component_custom_data_i*)io_api_manager->find_first(
          IO_COMPONENT_CUSTOM_DATA_API_NAME);
  io_component_tag = (const io_component_tag_i*)io_api_manager->find_first(
      IO_COMPONENT_TAG_API_NAME);
  io_component_flipbook_animation =
      (const io_component_flipbook_animation_i*)io_api_manager->find_first(
          IO_COMPONENT_FLIPBOOK_ANIMATION_API_NAME);
  io_component_post_effect_volume =
      (const io_component_post_effect_volume_i*)io_api_manager->find_first(
          IO_COMPONENT_POST_EFFECT_VOLUME_API_NAME);
  io_component_camera =
      (const io_component_camera_i*)io_api_manager->find_first(
          IO_COMPONENT_CAMERA_API_NAME);
  io_component_script =
      (const io_component_script_i*)io_api_manager->find_first(
          IO_COMPONENT_SCRIPT_API_NAME);
  io_component_voxel_shape =
      (const io_component_voxel_shape_i*)io_api_manager->find_first(
          IO_COMPONENT_VOXEL_SHAPE_API_NAME);
  io_component_light = (const io_component_light_i*)io_api_manager->find_first(
      IO_COMPONENT_LIGHT_API_NAME);
  io_component_node = (const io_component_node_i*)io_api_manager->find_first(
      IO_COMPONENT_NODE_API_NAME);
  io_component_character_controller =
      (const io_component_character_controller_i*)io_api_manager->find_first(
          IO_COMPONENT_CHARACTER_CONTROLLER_API_NAME);
  io_component_camera_controller =
      (const io_component_camera_controller_i*)io_api_manager->find_first(
          IO_COMPONENT_CAMERA_CONTROLLER_API_NAME);
  io_component_particle =
      (const io_component_particle_i*)io_api_manager->find_first(
          IO_COMPONENT_PARTICLE_API_NAME);

  // Factory plugin intefaces
  io_plugin_terrain = (const io_plugin_terrain_i*)io_api_manager->find_first(
      IO_PLUGIN_TERRAIN_API_NAME);

  // Set up the functions we provide
  io_user_script = {};
  {
    io_user_script.on_activate_scripts = on_activate_scripts;
    io_user_script.on_deactivate_scripts = on_deactivate_scripts;
    io_user_script.on_init_script = on_init_script;
    io_user_script.on_destroy_script = on_destroy_script;
    io_user_script.on_tick_scripts = on_tick_scripts;
  }
  io_api_manager->register_api(IO_USER_SCRIPT_API_NAME, &io_user_script);

  io_user_events = {};
  {
    io_user_events.on_physics_events = on_physics_events;
  }
  io_api_manager->register_api(IO_USER_EVENTS_API_NAME, &io_user_events);

  // Watch the scripts directory for changes
  io_filesystem->watch_data_source_directory("media/scripts",
                                             on_script_changed);

  return 0;
}

//----------------------------------------------------------------------------//
IO_API_EXPORT void IO_API_CALL unload_plugin()
{
  io_filesystem->remove_directory_watch(on_script_changed);
  io_api_manager->unregister_api(&io_user_script);
  io_api_manager->unregister_api(&io_user_events);
}
