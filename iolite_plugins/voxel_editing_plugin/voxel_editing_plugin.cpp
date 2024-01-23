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

#include "common.h"
#include "editing_ui.h"

//----------------------------------------------------------------------------//
static void on_tick_edit_tool(io_float32_t delta_t)
{
  editing_ui::show_editing_toolbar();
  editing_tools::handle_tool(editing_ui::current_tool_params);
  // benchmark::occupancy_mask();
}

//----------------------------------------------------------------------------//
IO_API_EXPORT uint32_t IO_API_CALL get_api_version() { return IO_API_VERSION; }

//----------------------------------------------------------------------------//
IO_API_EXPORT int IO_API_CALL load_plugin(void* api_manager)
{
  io_api_manager = (const io_api_manager_i*)api_manager;

  // Retrieve the interfaces we use
  {
    io_base = (const io_base_i*)io_api_manager->find_first(IO_BASE_API_NAME);
    io_logging =
        (const io_logging_i*)io_api_manager->find_first(IO_LOGGING_API_NAME);
    io_editor =
        (const io_editor_i*)io_api_manager->find_first(IO_EDITOR_API_NAME);
    io_world = (const io_world_i*)io_api_manager->find_first(IO_WORLD_API_NAME);
    io_debug_geometry = (const io_debug_geometry_i*)io_api_manager->find_first(
        IO_DEBUG_GEOMETRY_API_NAME);
    io_input_system = (const io_input_system_i*)io_api_manager->find_first(
        IO_INPUT_SYSTEM_API_NAME);
    io_component_node = (const io_component_node_i*)io_api_manager->find_first(
        IO_COMPONENT_NODE_API_NAME);
    io_component_voxel_shape =
        (const io_component_voxel_shape_i*)io_api_manager->find_first(
            IO_COMPONENT_VOXEL_SHAPE_API_NAME);
    io_resource_palette =
        (const io_resource_palette_i*)io_api_manager->find_first(
            IO_RESOURCE_PALETTE_API_NAME);
  }

  // Register the edit tool
  io_user_editor_tool = {};
  {
    io_user_editor_tool.on_tick = on_tick_edit_tool;
    io_user_editor_tool.get_icon = []() { return ICON_FA_CUBES; };
    io_user_editor_tool.get_tooltip = []() {
      return "Voxel editor for single voxel shapes.";
    };
  }
  io_api_manager->register_api(IO_USER_EDITOR_TOOL_API_NAME,
                               &io_user_editor_tool);

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
  io_api_manager->unregister_api(&io_user_editor_tool);
}
