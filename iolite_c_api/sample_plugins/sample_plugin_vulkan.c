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
#include <string.h>

// API
#include "iolite_api.h"

// Vulkan
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_core.h"

// Dependencies
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

// Interfaces we use
//----------------------------------------------------------------------------//
const struct io_api_manager_i* io_api_manager;
const struct io_logging_i* io_logging;
const struct io_low_level_vulkan_i* io_low_level_vulkan;

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
  io_low_level_vulkan =
      io_api_manager->find_first(IO_LOW_LEVEL_VULKAN_API_NAME);

  // Log some info about the physical device
  {
    VkPhysicalDevice vk_device = io_low_level_vulkan->get_vk_physical_device();
    VkInstance vk_instance = io_low_level_vulkan->get_vk_instance();

    // Retrieve Vulkan-internal functions via API
    io_low_level_vulkan_functions_t functions;
    io_low_level_vulkan->get_functions(&functions);
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
        functions.vk_get_instance_proc_addr;
    PFN_vkGetInstanceProcAddr vkGetDeviceProcAddr =
        functions.vk_get_device_proc_addr;

    // Fetch the function ptrs we need
    //   NOTE: It's also possible to statically link
    //         the Vulkan library instead of requesting all functions
    //         dynamically
    PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties =
        (PFN_vkGetPhysicalDeviceProperties)vkGetInstanceProcAddr(
            vk_instance, "vkGetPhysicalDeviceProperties");

    // Log the name of the physical device
    {
      VkPhysicalDeviceProperties props;
      vkGetPhysicalDeviceProperties(vk_device, &props);

      char buffer[1024u];
      stbsp_snprintf(buffer, sizeof(buffer), "Vulkan Sample Plugin: %s",
                     props.deviceName);
      io_logging->log_info(buffer);
    }
  }

  return 0;
}

//----------------------------------------------------------------------------//
IO_API_EXPORT void IO_API_CALL unload_plugin() {}
