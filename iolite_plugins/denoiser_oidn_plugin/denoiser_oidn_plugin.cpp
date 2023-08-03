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

// Dependencies
#include "oidn.h"

// API
#include "iolite_api.h"

// Interfaces we use
//----------------------------------------------------------------------------//
const io_api_manager_i* io_api_manager = nullptr;
const io_logging_i* io_logging = nullptr;

// Interfaces we provide
//----------------------------------------------------------------------------//
static io_user_denoiser_i io_user_denoiser = {};

//----------------------------------------------------------------------------//
static void denoise(io_uint32_t width, io_uint32_t height,
                    const io_vec4_t* input, io_vec4_t* output)
{
  OIDNDevice device = oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
  oidnCommitDevice(device);

  OIDNFilter filter = oidnNewFilter(device, "RT");
  oidnSetSharedFilterImage(filter, "color", (void*)input, OIDN_FORMAT_FLOAT3,
                           width, height, 0, 16u, 0);
  oidnSetSharedFilterImage(filter, "output", output, OIDN_FORMAT_FLOAT3, width,
                           height, 0, 16u, 0);
  oidnSetFilter1b(filter, "hdr", true);
  oidnCommitFilter(filter);

  oidnExecuteFilter(filter);

  const char* error_message;
  if (oidnGetDeviceError(device, &error_message) != OIDN_ERROR_NONE)
    io_logging->log_warning(error_message);

  oidnReleaseFilter(filter);
  oidnReleaseDevice(device);
}

//----------------------------------------------------------------------------//
static const char* get_name() { return "OIDN"; }

//----------------------------------------------------------------------------//
IO_API_EXPORT io_uint32_t IO_API_CALL get_api_version()
{
  return IO_API_VERSION;
}

//----------------------------------------------------------------------------//
#ifdef IO_PLUGIN_ID
//----------------------------------------------------------------------------//
IO_API_EXPORT io_uint64_t IO_API_CALL get_plugin_id() { return IO_PLUGIN_ID; }
//----------------------------------------------------------------------------//
#endif
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
IO_API_EXPORT int IO_API_CALL load_plugin(void* api_manager)
{
  io_api_manager = (const io_api_manager_i*)api_manager;

  // Core intefaces
  io_logging =
      (const io_logging_i*)io_api_manager->find_first(IO_LOGGING_API_NAME);

  // Register the interfaces we provide
  {
    io_user_denoiser.get_name = get_name;
    io_user_denoiser.denoise = denoise;

    io_api_manager->register_api(IO_USER_DENOISER_API_NAME, &io_user_denoiser);
  }

  return 0;
}

//----------------------------------------------------------------------------//
IO_API_EXPORT void IO_API_CALL unload_plugin()
{
  io_api_manager->unregister_api(&io_user_denoiser);
}
