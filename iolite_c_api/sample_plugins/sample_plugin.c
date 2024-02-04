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
#include <string.h>

// Dependencies
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

// API
#include "iolite_api.h"

// Interfaces we use
//----------------------------------------------------------------------------//
const struct io_api_manager_i* io_api_manager;
const struct io_logging_i* io_logging;
const struct io_entity_i* io_entity;
const struct io_component_node_i* io_component_node;
const struct io_world_i* io_world;
const struct io_base_i* io_base;

// Interfaces we provide
//----------------------------------------------------------------------------//
struct io_user_task_i io_user_task;

//----------------------------------------------------------------------------//
inline static void log_message(const char* fmt, ...)
{
  char buffer[256];

  va_list args;
  va_start(args, fmt);
  stbsp_vsnprintf(buffer, 256, fmt, args);
  va_end(args);

  io_logging->log_plugin("Sample C", buffer);
}

//----------------------------------------------------------------------------//
void traverse_node(io_ref_t node)
{
  if (!io_ref_is_valid(node))
    return;

  const io_ref_t entity = io_component_node->base.get_entity(node);
  const io_uint32_t depth = io_component_node->get_depth(node);

  // Log the name of the entity
  {
    char indents[16u];

    // Prepare indents for depth
    {
      const io_uint32_t num_indents_wanted = depth * 2u + 1u;
      const io_uint32_t num_indents_max = sizeof(indents) - 1u;
      const io_uint32_t num_indents = num_indents_wanted < num_indents_max
                                          ? num_indents_wanted
                                          : num_indents_max;

      io_uint32_t i;
      for (i = 0u; i < num_indents; ++i)
        indents[i] = i < num_indents - 1u ? ' ' : '-';
      indents[num_indents] = '\0';
    }

    log_message(" %s'%s' (Depth: %u)", indents, io_entity->get_name(entity),
                depth);
  }

  // Depth first traversal
  traverse_node(io_component_node->get_first_child(node));
  traverse_node(io_component_node->get_next_sibling(node));
}

//----------------------------------------------------------------------------//
void tick(float delta_t)
{
  static io_uint32_t num_messages_logged = 0u;

  // Log up to 5 messages
  if (num_messages_logged < 5u)
  {
    ++num_messages_logged;
    log_message("Your plugin is ticking, this is message %u/5...",
                num_messages_logged);
  }
}

//----------------------------------------------------------------------------//
void on_activate()
{
  io_logging->log_plugin("Sample C", "Game mode activated!");

  io_logging->log_plugin("Sample C", "Traversing all nodes in the world...");

  traverse_node(io_world->get_root_node());

  io_logging->log_plugin("Sample C", "Done!");
}

//----------------------------------------------------------------------------//
IO_API_EXPORT io_uint32_t IO_API_CALL get_api_version()
{
  return IO_API_VERSION;
}

//----------------------------------------------------------------------------//
IO_API_EXPORT int IO_API_CALL load_plugin(const void* api_manager)
{
  io_api_manager = api_manager;

  // Retrieve required interfaces
  io_logging = io_api_manager->find_first(IO_LOGGING_API_NAME);
  io_world = io_api_manager->find_first(IO_WORLD_API_NAME);
  io_entity = io_api_manager->find_first(IO_ENTITY_API_NAME);
  io_component_node = io_api_manager->find_first(IO_COMPONENT_NODE_API_NAME);
  io_base = io_api_manager->find_first(IO_BASE_API_NAME);

  // Set up and register user task
  memset(&io_user_task, 0, sizeof(io_user_task));
  io_user_task.on_tick = tick;
  io_user_task.on_activate = on_activate;
  io_api_manager->register_api(IO_USER_TASK_API_NAME, &io_user_task);

  return 0;
}

//----------------------------------------------------------------------------//
IO_API_EXPORT void IO_API_CALL unload_plugin()
{
  // Unregister our user task
  io_api_manager->unregister_api(&io_user_task);
}
