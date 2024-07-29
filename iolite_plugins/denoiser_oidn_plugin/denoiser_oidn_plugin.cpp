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
#include <cstring>

// API
#define _IO_PLUGIN_NAME "DenoiserOIDN"
#include "iolite_api.h"
#include "iolite_plugins_common.h"

// Interfaces we use
//----------------------------------------------------------------------------//
static const io_api_manager_i* io_api_manager = nullptr;
static const io_logging_i* io_logging = nullptr;

// Interfaces we provide
//----------------------------------------------------------------------------//
static io_user_denoiser_i io_user_denoiser = {};

//----------------------------------------------------------------------------//
static struct oidn_t
{
  // NOLINTBEGIN
  using oidn_set_device_int_t = void (*)(OIDNDevice device, const char* name,
                                         int value);
  using oidn_new_device_t = OIDNDevice (*)(OIDNDeviceType type);
  using oidn_commit_device_t = void (*)(OIDNDevice device);
  using oidn_new_filter_t = OIDNFilter (*)(OIDNDevice device, const char* type);
  using oidn_new_buffer_t = OIDNBuffer (*)(OIDNDevice device, size_t byteSize);
  using oidn_set_filter_image_t = void (*)(OIDNFilter filter, const char* name,
                                           OIDNBuffer buffer, OIDNFormat format,
                                           size_t width, size_t height,
                                           size_t byteOffset,
                                           size_t pixelByteStride,
                                           size_t rowByteStride);
  using oidn_set_shared_filter_image_t =
      void (*)(OIDNFilter filter, const char* name, void* devPtr,
               OIDNFormat format, size_t width, size_t height,
               size_t byteOffset, size_t pixelByteStride, size_t rowByteStride);
  using oidn_set_filter_bool_t = void (*)(OIDNFilter filter, const char* name,
                                          bool value);
  using oidn_set_filter_int_t = void (*)(OIDNFilter filter, const char* name,
                                         int value);
  using oidn_set_filter_progress_monitor_function_t = void (*)(
      OIDNFilter filter, OIDNProgressMonitorFunction func, void* userPtr);
  using oidn_commit_filter_t = void (*)(OIDNFilter filter);
  using oidn_execute_filter_t = void (*)(OIDNFilter filter);
  using oidn_get_device_error_t = OIDNError (*)(OIDNDevice device,
                                                const char** outMessage);
  using oidn_get_buffer_data_t = void* (*)(OIDNBuffer buffer);
  using oidn_read_buffer_t = void (*)(OIDNBuffer buffer, size_t byteOffset,
                                      size_t byteSize, void* dstHostPtr);
  using oidn_write_buffer_t = void (*)(OIDNBuffer buffer, size_t byteOffset,
                                       size_t byteSize, const void* srcHostPtr);
  using oidn_release_filter_t = void (*)(OIDNFilter filter);
  using oidn_release_buffer_t = void (*)(OIDNBuffer buffer);
  using oidn_release_device_t = void (*)(OIDNDevice device);

  oidn_set_device_int_t oidnSetDeviceInt;
  oidn_new_device_t oidnNewDevice;
  oidn_commit_device_t oidnCommitDevice;
  oidn_new_filter_t oidnNewFilter;
  oidn_new_buffer_t oidnNewBuffer;
  oidn_set_filter_image_t oidnSetFilterImage;
  oidn_set_shared_filter_image_t oidnSetSharedFilterImage;
  oidn_set_filter_bool_t oidnSetFilterBool;
  oidn_set_filter_int_t oidnSetFilterInt;
  oidn_set_filter_progress_monitor_function_t
      oidnSetFilterProgressMonitorFunction;
  oidn_commit_filter_t oidnCommitFilter;
  oidn_execute_filter_t oidnExecuteFilter;
  oidn_get_device_error_t oidnGetDeviceError;
  oidn_read_buffer_t oidnReadBuffer;
  oidn_write_buffer_t oidnWriteBuffer;
  oidn_get_buffer_data_t oidnGetBufferData;
  oidn_release_filter_t oidnReleaseFilter;
  oidn_release_buffer_t oidnReleaseBuffer;
  oidn_release_device_t oidnReleaseDevice;
  // NOLINTEND

  void* oidn{nullptr};

  bool init{false};
} oidn;

//----------------------------------------------------------------------------//
static io_bool_t unload_oidn(bool error = false)
{
  if (error)
    common::log_message(io_logging, "Failed to load OIDN library!");

  if (oidn.oidn)
    common::unload_library(oidn.oidn);

  memset(&oidn, 0, sizeof(oidn));
  return !error;
};

//----------------------------------------------------------------------------//
static io_bool_t load_oidn()
{
  if (oidn.init)
    return true;

  oidn.oidn = common::load_library("OpenImageDenoise");
  if (!oidn.oidn)
    return unload_oidn(true);

#define LOAD_FUNCTION(_function, _type)                                        \
  oidn._function =                                                             \
      (oidn_t::_type)common::load_function(oidn.oidn, #_function);             \
  if (!oidn._function)                                                         \
    return unload_oidn(true);

  LOAD_FUNCTION(oidnSetDeviceInt, oidn_set_device_int_t);
  LOAD_FUNCTION(oidnNewDevice, oidn_new_device_t);
  LOAD_FUNCTION(oidnCommitDevice, oidn_commit_device_t);
  LOAD_FUNCTION(oidnNewFilter, oidn_new_filter_t);
  LOAD_FUNCTION(oidnNewBuffer, oidn_new_buffer_t);
  LOAD_FUNCTION(oidnSetFilterImage, oidn_set_filter_image_t);
  LOAD_FUNCTION(oidnSetSharedFilterImage, oidn_set_shared_filter_image_t);
  LOAD_FUNCTION(oidnSetFilterBool, oidn_set_filter_bool_t);
  LOAD_FUNCTION(oidnSetFilterInt, oidn_set_filter_int_t);
  LOAD_FUNCTION(oidnSetFilterProgressMonitorFunction,
                oidn_set_filter_progress_monitor_function_t);
  LOAD_FUNCTION(oidnCommitFilter, oidn_commit_filter_t);
  LOAD_FUNCTION(oidnExecuteFilter, oidn_execute_filter_t);
  LOAD_FUNCTION(oidnGetDeviceError, oidn_get_device_error_t);
  LOAD_FUNCTION(oidnGetBufferData, oidn_get_buffer_data_t);
  LOAD_FUNCTION(oidnReadBuffer, oidn_read_buffer_t);
  LOAD_FUNCTION(oidnWriteBuffer, oidn_write_buffer_t);
  LOAD_FUNCTION(oidnReleaseFilter, oidn_release_filter_t);
  LOAD_FUNCTION(oidnReleaseBuffer, oidn_release_buffer_t);
  LOAD_FUNCTION(oidnReleaseDevice, oidn_release_device_t);

#undef LOAD_FUNCTION

  return true;
};

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
  if (!load_oidn())
    return;

  common::log_message(io_logging, "Denoising image...");

  OIDNDevice device = oidn.oidnNewDevice(OIDN_DEVICE_TYPE_DEFAULT);
  // oidn.oidnSetDeviceInt(device, "verbose", 4);
  oidn.oidnCommitDevice(device);

  OIDNBuffer input_buffer =
      oidn.oidnNewBuffer(device, width * height * sizeof(*input));
  OIDNBuffer output_buffer =
      oidn.oidnNewBuffer(device, width * height * sizeof(*output));

  oidn.oidnWriteBuffer(input_buffer, 0u, width * height * sizeof(*input),
                       input);
  oidn.oidnWriteBuffer(output_buffer, 0u, width * height * sizeof(*input),
                       input);

  OIDNFilter filter = oidn.oidnNewFilter(device, "RT");
  oidn.oidnSetFilterImage(filter, "color", input_buffer, OIDN_FORMAT_FLOAT3,
                          width, height, 0, 16u, 0);
  oidn.oidnSetFilterImage(filter, "output", output_buffer, OIDN_FORMAT_FLOAT3,
                          width, height, 0, 16u, 0);
  oidn.oidnSetFilterBool(filter, "hdr", true);
  // oidn.oidnSetFilterInt(filter, "quality", OIDN_QUALITY_HIGH);
  oidn.oidnSetFilterProgressMonitorFunction(filter, on_progress_report,
                                            nullptr);
  oidn.oidnCommitFilter(filter);
  oidn.oidnExecuteFilter(filter);

  const char* error_message;
  if (oidn.oidnGetDeviceError(device, &error_message) != OIDN_ERROR_NONE)
  {
    common::log_message(io_logging, "OIDN ERROR: %s", error_message);
  }
  else
  {
    oidn.oidnReadBuffer(output_buffer, 0u, width * height * sizeof(*output),
                        output);
  }

  oidn.oidnReleaseBuffer(input_buffer);
  oidn.oidnReleaseBuffer(output_buffer);

  oidn.oidnReleaseFilter(filter);
  oidn.oidnReleaseDevice(device);

  common::log_message(io_logging, "Done!");

  unload_oidn();
}

//----------------------------------------------------------------------------//
static const char* get_name() { return "OIDN"; }

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

  if (!load_oidn())
  {
    // Unsupported if OIDN is not available
    return -1;
  }
  unload_oidn();

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
