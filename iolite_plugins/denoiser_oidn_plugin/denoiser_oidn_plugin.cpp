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
#include <cstring>

// API
#define _IO_PLUGIN_NAME "DenoiserOIDN"
#include "iolite_api.h"
#include "iolite_plugins_libraries.h"

// Interfaces we use
//----------------------------------------------------------------------------//
static const io_api_manager_i* io_api_manager = nullptr;
static const io_logging_i* io_logging = nullptr;

// Interfaces we provide
//----------------------------------------------------------------------------//
static io_user_denoiser_i io_user_denoiser = {};

//----------------------------------------------------------------------------//
static const char* get_name() { return "OIDN"; }

//----------------------------------------------------------------------------//
static bool on_progress_report(void* user_ptr, double n)
{
  common::log_message(io_logging, "  Progress: %.2f %%", (float)n * 100.0f);
  return true;
}

//----------------------------------------------------------------------------//
static void denoise(io_uint32_t width, io_uint32_t height,
                    const io_vec4_t* input, io_vec4_t* output)
{
  if (!libraries::load_oidn(io_logging))
    return;

  common::log_message(io_logging, "Denoising image...");

  OIDNDevice device = libraries::oidn.oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
  // libraries::oidn.oidnSetDeviceInt(device, "verbose", 4);
  libraries::oidn.oidnCommitDevice(device);

  OIDNBuffer input_buffer =
      libraries::oidn.oidnNewBuffer(device, width * height * sizeof(*input));
  OIDNBuffer output_buffer =
      libraries::oidn.oidnNewBuffer(device, width * height * sizeof(*output));

  libraries::oidn.oidnWriteBuffer(input_buffer, 0u,
                                  width * height * sizeof(*input), input);
  libraries::oidn.oidnWriteBuffer(output_buffer, 0u,
                                  width * height * sizeof(*input), input);

  OIDNFilter filter = libraries::oidn.oidnNewFilter(device, "RT");
  libraries::oidn.oidnSetFilterImage(filter, "color", input_buffer,
                                     OIDN_FORMAT_FLOAT3, width, height, 0, 16u,
                                     0);
  libraries::oidn.oidnSetFilterImage(filter, "output", output_buffer,
                                     OIDN_FORMAT_FLOAT3, width, height, 0, 16u,
                                     0);
  libraries::oidn.oidnSetFilterBool(filter, "hdr", true);
  // libraries::oidn.oidnSetFilterInt(filter, "quality", OIDN_QUALITY_HIGH);
  libraries::oidn.oidnSetFilterProgressMonitorFunction(
      filter, on_progress_report, nullptr);
  libraries::oidn.oidnCommitFilter(filter);
  libraries::oidn.oidnExecuteFilter(filter);

  const char* error_message;
  if (libraries::oidn.oidnGetDeviceError(device, &error_message) !=
      OIDN_ERROR_NONE)
  {
    common::log_message(io_logging, "OIDN ERROR: %s", error_message);
  }
  else
  {
    libraries::oidn.oidnReadBuffer(output_buffer, 0u,
                                   width * height * sizeof(*output), output);
  }

  libraries::oidn.oidnReleaseBuffer(input_buffer);
  libraries::oidn.oidnReleaseBuffer(output_buffer);

  libraries::oidn.oidnReleaseFilter(filter);
  libraries::oidn.oidnReleaseDevice(device);

  common::log_message(io_logging, "Done!");

  libraries::unload_oidn(io_logging);
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

  // Retrieve API interfaces
  io_logging =
      (const io_logging_i*)io_api_manager->find_first(IO_LOGGING_API_NAME);

  if (!libraries::load_oidn(io_logging))
    // Unsupported if OIDN is not available
    return -1;
  libraries::unload_oidn(io_logging);

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
