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
const io_animation_system_i* io_animation_system = {};
const io_sound_i* io_sound = {};
const io_input_system_i* io_input_system = {};
const io_plugin_terrain_i* io_plugin_terrain = {};
const io_physics_i* io_physics = {};
const io_debug_geometry_i* io_debug_geometry = {};
const io_pathfinding_i* io_pathfinding = {};
const io_custom_components_i* io_custom_components = {};
const io_custom_event_streams_i* io_custom_event_streams = {};

const io_component_node_i* io_component_node = {};
const io_component_custom_data_i* io_component_custom_data = {};
const io_component_tag_i* io_component_tag = {};
const io_component_flipbook_animation_i* io_component_flipbook_animation = {};
const io_component_post_effect_volume_i* io_component_post_effect_volume = {};
const io_component_camera_i* io_component_camera = {};
const io_component_voxel_shape_i* io_component_voxel_shape = {};
const io_component_light_i* io_component_light = {};
const io_component_character_controller_i* io_component_character_controller =
    {};
const io_component_camera_controller_i* io_component_camera_controller = {};
const io_component_particle_i* io_component_particle = {};
const io_component_vehicle_i* io_component_vehicle = {};
const io_component_vehicle_wheel_i* io_component_vehicle_wheel = {};
const io_component_joint_i* io_component_joint = {};

const io_resource_palette_i* io_resource_palette = {};

// Interfaces we provide
//----------------------------------------------------------------------------//
io_user_events_i io_user_events = {};
io_user_task_i io_user_task = {};

// Custom components
//----------------------------------------------------------------------------//
io_handle16_t script_manager = {};

// Custom event streams
//----------------------------------------------------------------------------//
io_handle16_t event_stream = {};

// SOA style batch of script data
//----------------------------------------------------------------------------//
struct script_batch_t
{
  inline sol::state** get_states() const { return *states; }
  inline io_ref_t* get_entities() const { return *entities; }
  inline io_uint32_t* get_update_intervals() const { return *update_intervals; }
  inline io_name_t* get_names() const { return *names; }

  uint32_t num_scripts;

  sol::state*** states;
  io_ref_t** entities;
  io_uint32_t** update_intervals;
  io_name_t** names;
};

//----------------------------------------------------------------------------//
static script_batch_t get_batch()
{
  script_batch_t batch{};
  {
    batch.entities =
        io_custom_components->get_entity_memory_ptr(script_manager);
    batch.states = (sol::state***)io_custom_components->get_property_memory_ptr(
        script_manager, "state");
    batch.update_intervals =
        (uint32_t**)io_custom_components->get_property_memory_ptr(
            script_manager, "updateInterval");
    batch.names = (io_name_t**)io_custom_components->get_property_memory_ptr(
        script_manager, "scriptName");

    batch.num_scripts =
        io_custom_components->get_num_active_components(script_manager);
  }
  return batch;
}

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
      io_logging->log_plugin("Lua", string_buffer);                            \
    }                                                                          \
  }

//----------------------------------------------------------------------------//
static std::vector<std::string> queued_world_loads;
static std::vector<io_ref_t> queued_nodes_to_destroy;

//----------------------------------------------------------------------------//
struct lua_event_listener_t
{
  io_ref_t target_entity;
  std::vector<io_name_t> event_types;
};
static std::vector<lua_event_listener_t> event_listeners;

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
  // Clear upfront to avoid recursions
  auto temp_worlds_to_load = queued_world_loads;
  queued_world_loads.clear();
  auto temp_nodes_to_destroy = queued_nodes_to_destroy;
  queued_nodes_to_destroy.clear();

  for (auto node : temp_nodes_to_destroy)
    io_component_node->base.destroy(node);
  for (const auto& world_name : temp_worlds_to_load)
    io_world->load_world(world_name.c_str());
}

//----------------------------------------------------------------------------//
auto load_script(sol::state& state,
                 const char* filepath) -> sol::protected_function
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
      io_logging->log_plugin("Lua", string_buffer);
    }
    else
    {
      stbsp_snprintf(string_buffer, string_buffer_length,
                     "Lua script '%s' loaded...", filepath);
      io_logging->log_plugin("Lua", string_buffer);

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

//----------------------------------------------------------------------------//
void script_tick_physics(sol::state& state, io_float32_t delta_t,
                         io_ref_t entity)
{
  if (!scripts_active || !state["__ScriptIsActive"].get<bool>())
    return;

  sol::protected_function tick = state["TickPhysics"];
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

//----------------------------------------------------------------------------//
lua_event_listener_t* find_event_listener(io_ref_t target_entity)
{
  // Try to locate an existing entry
  lua_event_listener_t* listener = nullptr;
  for (auto& l : event_listeners)
  {
    if (io_ref_is_equal(l.target_entity, target_entity))
    {
      listener = &l;
      break;
    }
  }

  return listener;
}

//----------------------------------------------------------------------------//
void register_event_listener(io_ref_t target_entity, const char* event_type)
{
  const io_name_t event_type_name = io_to_name(event_type);

  // Create new one if none found
  lua_event_listener_t* listener = find_event_listener(target_entity);
  if (!listener)
  {
    event_listeners.resize(event_listeners.size() + 1u);
    listener = &event_listeners.back();
    listener->target_entity = target_entity;
  }

  // Add new event type to listener if not already available
  io_bool_t found = false;

  for (const auto etn : listener->event_types)
  {
    if (io_name_is_equal(event_type_name, etn))
    {
      found = true;
      break;
    }
  }

  if (!found)
    listener->event_types.push_back(event_type_name);
}

//----------------------------------------------------------------------------//
void unregister_event_listener(io_ref_t target_entity, const char* event_type)
{
  const io_name_t event_type_name = io_to_name(event_type);

  lua_event_listener_t* listener = find_event_listener(target_entity);
  if (!listener)
    return;

  for (auto it = listener->event_types.begin();
       it != listener->event_types.end(); ++it)
  {
    if (io_name_is_equal(*it, event_type_name))
    {
      listener->event_types.erase(it);
      // Can only exist once so it is safe to break here
      break;
    }
  }
}

//----------------------------------------------------------------------------//
void post_event(io_ref_t source_entity, const char* event_type,
                io_variant_t* variants, io_size_t variants_length,
                io_ref_t* target_entities, io_size_t target_entities_length)
{
  // Allocate event
  lua_user_event_t::event_data_t* event =
      (lua_user_event_t::event_data_t*)
          io_custom_event_streams->post_event_uninitialized(
              event_stream, event_type,
              sizeof(lua_user_event_t::event_data_t) +
                  sizeof(io_variant_t) * variants_length +
                  sizeof(io_ref_t) * target_entities_length);

  // Copy payload
  const auto variants_ptr = (io_variant_t*)(event + 1u);
  memcpy(variants_ptr, variants, variants_length * sizeof(io_variant_t));
  const auto target_entities_ptr = (io_ref_t*)(variants_ptr + variants_length);
  memcpy(target_entities_ptr, target_entities,
         target_entities_length * sizeof(io_ref_t));

  // Set other metadata
  event->variants = lua_array_wrapper_t(variants_ptr, variants_length);
  event->target_entities =
      lua_array_wrapper_t(target_entities_ptr, target_entities_length);
  event->source_entity = source_entity;
}

//----------------------------------------------------------------------------//
void script_dispatch_user_events(sol::state& state, io_ref_t entity)
{
  if (!scripts_active || !state["__ScriptIsActive"].get<bool>())
    return;

  auto listener = find_event_listener(entity);
  if (!listener)
    return;

  // Collect relevant events
  std::vector<lua_user_event_t> user_events;
  {
    const io_events_header_t *event, *end;
    io_custom_event_streams->process_events(event_stream, &event, &end);

    while (event < end)
    {
      // Check if we need to handle this event
      bool handle_event = false;
      for (auto t : listener->event_types)
      {
        if (io_name_is_equal(t, event->type))
        {
          handle_event = true;
          break;
        }
      }

      if (handle_event)
      {
        const auto data =
            *(lua_user_event_t::event_data_t*)io_events_get_data(event);

        bool event_filtered = false;

        // Filter target entities upfront (if provided)
        if (!data.target_entities.empty())
        {
          bool found = false;
          for (auto it = data.target_entities.begin();
               it != data.target_entities.end(); ++it)
          {
            if (io_ref_is_equal(entity, *it))
            {
              found = true;
              break;
            }
          }

          event_filtered = !found;
        }

        if (!event_filtered)
        {
          lua_user_event_t user_event;
          {
            user_event.data = data;
            user_event.type = io_base->name_get_string(event->type);
          }
          user_events.emplace_back(user_event);
        }
      }

      event = io_events_get_next(event);
    }
  }

  // Finally dispatch the events
  if (!user_events.empty())
  {
    sol::protected_function on_user_event = state["OnUserEvent"];
    if (on_user_event.valid())
      SOL_VALIDATE_RESULT(on_user_event(entity, user_events),
                          state["__ScriptName"].get<const char*>());
  }
}

//----------------------------------------------------------------------------//
static void dispatch_user_events(const script_batch_t* batch)
{
  // Dispatch events to all listeners
  for (uint32_t i = 0u; i < batch->num_scripts; ++i)
  {
    auto state = batch->get_states()[i];
    script_dispatch_user_events(*state, batch->get_entities()[i]);
  }

  // Reset the event stream
  io_custom_event_streams->reset(event_stream);

  // Clean up obsolete event listeners
  for (auto it = event_listeners.begin(); it != event_listeners.end();)
  {
    if (io_entity->is_alive(it->target_entity))
    {
      ++it;
      continue;
    }

    it = event_listeners.erase(it);
  }
}

//----------------------------------------------------------------------------//
static void on_script_init(const char* script_name, io_ref_t entity,
                           io_uint32_t update_interval, sol::state** s)
{
  void* mem = io_base->mem_allocate(sizeof(sol::state));
  *s = new (mem) sol::state();

  script_init_state(**s);

  if (strlen(script_name) > 0)
    execute_script(**s, "media/scripts", script_name);

  script_activate(**s, entity, update_interval);
  execute_queued_actions();
}

//----------------------------------------------------------------------------//
static void on_script_destroy(io_ref_t entity, sol::state** s)
{
  script_deactivate(**s, entity);
  execute_queued_actions();

  (*s)->~state();
  io_base->mem_free(*s);
  *s = nullptr;
}

//----------------------------------------------------------------------------//
static void on_scripts_activate(const script_batch_t* batch)
{
  scripts_active = true;

  for (uint32_t i = 0u; i < batch->num_scripts; ++i)
  {
    auto state = batch->get_states()[i];
    script_activate(*state, batch->get_entities()[i],
                    batch->get_update_intervals()[i]);
  }

  execute_queued_actions();
}

//----------------------------------------------------------------------------//
static void on_scripts_deactivate(const script_batch_t* batch)
{
  for (uint32_t i = 0u; i < batch->num_scripts; ++i)
  {
    auto state = batch->get_states()[i];
    script_deactivate(*state, batch->get_entities()[i]);
  }

  execute_queued_actions();
  scripts_active = false;
}

//----------------------------------------------------------------------------//
static void on_scripts_tick(io_float32_t delta_t, const script_batch_t* batch)
{
  for (uint32_t i = 0u; i < batch->num_scripts; ++i)
  {
    auto state = batch->get_states()[i];
    script_tick(*state, delta_t, batch->get_entities()[i]);
    script_update(*state, delta_t, batch->get_entities()[i],
                  batch->get_update_intervals()[i]);
  }

  dispatch_user_events(batch);
  execute_queued_actions();
}

//----------------------------------------------------------------------------//
static void on_scripts_tick_physics(io_float32_t delta_t,
                                    const script_batch_t* batch)
{
  for (uint32_t i = 0u; i < batch->num_scripts; ++i)
  {
    auto state = batch->get_states()[i];
    script_tick_physics(*state, delta_t, batch->get_entities()[i]);
  }

  execute_queued_actions();
}

//----------------------------------------------------------------------------//
static void on_script_changed(const char* filename, const char* filepath)
{
  const auto filename_without_extension =
      std::filesystem::path(filename).filename().replace_extension().u8string();
  const auto script_name =
      io_to_name((const char*)filename_without_extension.c_str());

  const auto batch = get_batch();
  for (uint32_t i = 0u; i < batch.num_scripts; ++i)
  {
    if (batch.get_names()[i].hash == script_name.hash)
    {
      execute_script(*batch.get_states()[i], "media/scripts",
                     (const char*)filename_without_extension.c_str());
    }
  }
}

//----------------------------------------------------------------------------//
static void on_physics_events(const io_events_header_t* begin,
                              const io_events_header_t* end)
{
  if (!scripts_active)
    return;

  static std::vector<lua_physics_contact_event_t> lua_contact_events;

  const io_events_header_t* event = begin;

  const char* contact_event_type_str = "physics_contact";
  const char* contact_type_touch_lost_str = "touch_lost";
  const char* contact_type_touch_found_str = "touch_found";
  const char* contact_type_trigger_touch_lost_str = "trigger_touch_lost";
  const char* contact_type_trigger_touch_found_str = "trigger_touch_found";

  const io_name_t contact_event_type = io_to_name(contact_event_type_str);
  const io_name_t contact_type_touch_lost =
      io_to_name(contact_type_touch_lost_str);
  const io_name_t contact_type_touch_found =
      io_to_name(contact_type_touch_found_str);
  const io_name_t contact_type_trigger_touch_lost =
      io_to_name(contact_type_trigger_touch_lost_str);
  const io_name_t contact_type_trigger_touch_found =
      io_to_name(contact_type_trigger_touch_found_str);

  // Collect events
  while (event < end)
  {
    if (event->type.hash == contact_event_type.hash)
    {
      auto* contact =
          (io_events_data_physics_contact_t*)io_events_get_data(event);

      if (contact->type.hash == contact_type_touch_lost.hash)
      {
        lua_contact_events.push_back(
            {contact_event_type_str,
             {contact->entity0, contact->entity1, contact->pos,
              contact->impulse, contact_type_touch_lost_str}});
      }
      else if (contact->type.hash == contact_type_touch_found.hash)
      {
        lua_contact_events.push_back(
            {contact_event_type_str,
             {contact->entity0, contact->entity1, contact->pos,
              contact->impulse, contact_type_touch_found_str}});
      }
      else if (contact->type.hash == contact_type_trigger_touch_lost.hash)
      {
        lua_contact_events.push_back(
            {contact_event_type_str,
             {contact->entity0, contact->entity1, contact->pos,
              contact->impulse, contact_type_trigger_touch_lost_str}});
      }
      else if (contact->type.hash == contact_type_trigger_touch_found.hash)
      {
        lua_contact_events.push_back(
            {contact_event_type_str,
             {contact->entity0, contact->entity1, contact->pos,
              contact->impulse, contact_type_trigger_touch_found_str}});
      }
    }

    event = io_events_get_next(event);
  }

  // Collect N events to dispatch this frame
  constexpr uint32_t num_events_to_dispatch_per_frame = 100u;
  std::vector<lua_physics_contact_event_t> contact_events_to_dispatch;
  {
    const auto first = lua_contact_events.begin();
    const auto last = lua_contact_events.begin() +
                      glm::min((uint32_t)lua_contact_events.size(),
                               num_events_to_dispatch_per_frame);
    contact_events_to_dispatch.insert(contact_events_to_dispatch.begin(), first,
                                      last);
    lua_contact_events.erase(first, last);

    // Clean up obsolete events
    for (auto it = contact_events_to_dispatch.begin();
         it != contact_events_to_dispatch.end();)
    {
      if (io_ref_is_valid(it->data.entity0) &&
          io_entity->is_alive(it->data.entity0) &&
          io_ref_is_valid(it->data.entity1) &&
          io_entity->is_alive(it->data.entity1))
      {
        ++it;
        continue;
      }

      it = contact_events_to_dispatch.erase(it);
    }
  }

  const auto batch = get_batch();
  for (uint32_t i = 0u; i < batch.num_scripts; ++i)
  {
    const auto& s = *batch.get_states()[i];
    if (!s["__ScriptIsActive"].get<bool>())
      return;

    sol::protected_function on_event = s["OnEvent"];
    if (on_event.valid())
      SOL_VALIDATE_RESULT(
          on_event(batch.get_entities()[i], contact_events_to_dispatch),
          s["__ScriptName"].get<const char*>());
  }
}

//----------------------------------------------------------------------------//
static void on_tick(float delta_t)
{
  const auto batch = get_batch();
  on_scripts_tick(delta_t, &batch);
}

//----------------------------------------------------------------------------//
static void on_tick_physics(float delta_t)
{
  const auto batch = get_batch();
  on_scripts_tick_physics(delta_t, &batch);
}

//----------------------------------------------------------------------------//
static void on_activate()
{
  const auto batch = get_batch();
  on_scripts_activate(&batch);
}

//----------------------------------------------------------------------------//
static void on_deactivate()
{
  const auto batch = get_batch();
  on_scripts_deactivate(&batch);
}

//----------------------------------------------------------------------------//
static void on_script_components_create(const io_ref_t* components,
                                        io_size_t components_length)
{
  const auto batch = get_batch();
  for (uint32_t i = 0u; i < components_length; ++i)
  {
    const auto script = components[i];
    const auto script_idx =
        io_custom_components->make_index(script_manager, script);

    on_script_init(io_base->name_get_string(batch.get_names()[script_idx]),
                   batch.get_entities()[script_idx],
                   batch.get_update_intervals()[script_idx],
                   &(batch.get_states()[script_idx]));
  }
}

//----------------------------------------------------------------------------//
static void on_script_components_destroy(const io_ref_t* components,
                                         io_size_t components_length)
{
  const auto batch = get_batch();
  for (uint32_t i = 0u; i < components_length; ++i)
  {
    const auto script = components[i];
    const auto script_idx =
        io_custom_components->make_index(script_manager, script);

    on_script_destroy(batch.get_entities()[script_idx],
                      &(batch.get_states()[script_idx]));
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

  // Core interfaces
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
  io_animation_system =
      (const io_animation_system_i*)io_api_manager->find_first(
          IO_ANIMATION_SYSTEM_API_NAME);
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
  io_custom_components =
      (const io_custom_components_i*)io_api_manager->find_first(
          IO_CUSTOM_COMPONENTS_API_NAME);
  io_custom_event_streams =
      (const io_custom_event_streams_i*)io_api_manager->find_first(
          IO_CUSTOM_EVENT_STREAMS_API_NAME);

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
  io_component_vehicle =
      (const io_component_vehicle_i*)io_api_manager->find_first(
          IO_COMPONENT_VEHICLE_API_NAME);
  io_component_vehicle_wheel =
      (const io_component_vehicle_wheel_i*)io_api_manager->find_first(
          IO_COMPONENT_VEHICLE_WHEEL_API_NAME);
  io_component_joint = (const io_component_joint_i*)io_api_manager->find_first(
      IO_COMPONENT_JOINT_API_NAME);

  io_resource_palette =
      (const io_resource_palette_i*)io_api_manager->find_first(
          IO_RESOURCE_PALETTE_API_NAME);

  // Factory plugin interfaces
  io_plugin_terrain = (const io_plugin_terrain_i*)io_api_manager->find_first(
      IO_PLUGIN_TERRAIN_API_NAME);

  // Register task
  io_user_task = {};
  {
    io_user_task.on_activate = on_activate;
    io_user_task.on_deactivate = on_deactivate;
    io_user_task.on_tick = on_tick;
    io_user_task.on_tick_physics = on_tick_physics;
  }
  io_api_manager->register_api(IO_USER_TASK_API_NAME, &io_user_task);

  // Register event callbacks
  io_user_events = {};
  {
    io_user_events.on_physics_events = on_physics_events;
  }
  io_api_manager->register_api(IO_USER_EVENTS_API_NAME, &io_user_events);

  // Set up our custom script component
  {
    script_manager = io_custom_components->request_manager();

    io_custom_components->register_property(
        script_manager, "scriptName", io_variant_from_string(""), nullptr, 0);
    io_custom_components->register_property(script_manager, "updateInterval",
                                            io_variant_from_uint(100u), nullptr,
                                            0);
    io_custom_components->register_property(
        script_manager, "state", io_variant_from_uint64(0ull), nullptr,
        io_property_flags_runtime_only);

    io_custom_components_callbacks_t callbacks = {};
    {
      callbacks.on_components_create = on_script_components_create;
      callbacks.on_components_destroy = on_script_components_destroy;
    }
    io_custom_components->register_callbacks(script_manager, &callbacks);
    io_custom_components->init_manager(script_manager, "Script");
  }

  // Set up custom event stream
  {
    event_stream = io_custom_event_streams->request_event_stream();
  }

  // Watch the "scripts" directory for changes
  io_filesystem->watch_data_source_directory("media/scripts",
                                             on_script_changed);

  return 0;
}

//----------------------------------------------------------------------------//
IO_API_EXPORT void IO_API_CALL unload_plugin()
{
  io_filesystem->remove_directory_watch(on_script_changed);
  io_api_manager->unregister_api(&io_user_events);
  io_api_manager->unregister_api(&io_user_task);
  io_custom_components->release_and_destroy_manager(script_manager);
  io_custom_event_streams->release_and_destroy_event_stream(event_stream);
}
