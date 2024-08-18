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

#pragma once

// Plugins
#include "iolite_api.h"
#include "iolite_plugins_common.h"

// Dependencies
#include "curl.h"
#include "oidn.h"

//----------------------------------------------------------------------------//
namespace libraries
{

// Open Image Denoise (OIDN)
//----------------------------------------------------------------------------//
static struct oidn_t
{
  // NOLINTBEGIN
  using pfn_oidnSetDeviceInt = void (*)(OIDNDevice device, const char* name,
                                        int value);
  using pfn_oidnNewDevice = OIDNDevice (*)(OIDNDeviceType type);
  using pfn_oidnCommitDevice = void (*)(OIDNDevice device);
  using pfn_oidnNewFilter = OIDNFilter (*)(OIDNDevice device, const char* type);
  using pfn_oidnNewBuffer = OIDNBuffer (*)(OIDNDevice device, size_t byteSize);
  using pfn_oidnSetFilterImage = void (*)(OIDNFilter filter, const char* name,
                                          OIDNBuffer buffer, OIDNFormat format,
                                          size_t width, size_t height,
                                          size_t byteOffset,
                                          size_t pixelByteStride,
                                          size_t rowByteStride);
  using pfn_oidnSetSharedFilterImage =
      void (*)(OIDNFilter filter, const char* name, void* devPtr,
               OIDNFormat format, size_t width, size_t height,
               size_t byteOffset, size_t pixelByteStride, size_t rowByteStride);
  using pfn_oidnSetFilterBool = void (*)(OIDNFilter filter, const char* name,
                                         bool value);
  using pfn_oidnSetFilterInt = void (*)(OIDNFilter filter, const char* name,
                                        int value);
  using pfn_oidnSetFilterProgressMonitorFunction = void (*)(
      OIDNFilter filter, OIDNProgressMonitorFunction func, void* userPtr);
  using pfn_oidnCommitFilter = void (*)(OIDNFilter filter);
  using pfn_oidnExecuteFilter = void (*)(OIDNFilter filter);
  using pfn_oidnGetDeviceError = OIDNError (*)(OIDNDevice device,
                                               const char** outMessage);
  using pfn_oidnGetBufferData = void* (*)(OIDNBuffer buffer);
  using pfn_oidnReadBuffer = void (*)(OIDNBuffer buffer, size_t byteOffset,
                                      size_t byteSize, void* dstHostPtr);
  using pfn_oidnWriteBuffer = void (*)(OIDNBuffer buffer, size_t byteOffset,
                                       size_t byteSize, const void* srcHostPtr);
  using pfn_oidnReleaseFilter = void (*)(OIDNFilter filter);
  using pfn_oidnReleaseBuffer = void (*)(OIDNBuffer buffer);
  using pfn_oidnReleaseDevice = void (*)(OIDNDevice device);

  pfn_oidnSetDeviceInt oidnSetDeviceInt;
  pfn_oidnNewDevice oidnNewDevice;
  pfn_oidnCommitDevice oidnCommitDevice;
  pfn_oidnNewFilter oidnNewFilter;
  pfn_oidnNewBuffer oidnNewBuffer;
  pfn_oidnSetFilterImage oidnSetFilterImage;
  pfn_oidnSetSharedFilterImage oidnSetSharedFilterImage;
  pfn_oidnSetFilterBool oidnSetFilterBool;
  pfn_oidnSetFilterInt oidnSetFilterInt;
  pfn_oidnSetFilterProgressMonitorFunction oidnSetFilterProgressMonitorFunction;
  pfn_oidnCommitFilter oidnCommitFilter;
  pfn_oidnExecuteFilter oidnExecuteFilter;
  pfn_oidnGetDeviceError oidnGetDeviceError;
  pfn_oidnReadBuffer oidnReadBuffer;
  pfn_oidnWriteBuffer oidnWriteBuffer;
  pfn_oidnGetBufferData oidnGetBufferData;
  pfn_oidnReleaseFilter oidnReleaseFilter;
  pfn_oidnReleaseBuffer oidnReleaseBuffer;
  pfn_oidnReleaseDevice oidnReleaseDevice;
  // NOLINTEND

  void* oidn{nullptr};
  bool init{false};
} oidn;

//----------------------------------------------------------------------------//
static io_bool_t unload_oidn(const io_logging_i* io_logging, bool error = false)
{
  if (error)
    common::log_message(io_logging, "Failed to load OIDN library!");

  if (oidn.oidn)
    common::unload_library(oidn.oidn);

  memset(&oidn, 0, sizeof(oidn));
  return !error;
};

//----------------------------------------------------------------------------//
static io_bool_t load_oidn(const io_logging_i* io_logging)
{
  if (oidn.init)
    return true;

  oidn.oidn = common::load_library("OpenImageDenoise");
  if (!oidn.oidn)
    return unload_oidn(io_logging, true);

#define LOAD_FUNCTION(_function)                                               \
  oidn._function =                                                             \
      (oidn_t::pfn_##_function)common::load_function(oidn.oidn, #_function);   \
  if (!oidn._function)                                                         \
    return unload_oidn(io_logging, true);

  LOAD_FUNCTION(oidnSetDeviceInt)
  LOAD_FUNCTION(oidnNewDevice);
  LOAD_FUNCTION(oidnCommitDevice);
  LOAD_FUNCTION(oidnNewFilter);
  LOAD_FUNCTION(oidnNewBuffer);
  LOAD_FUNCTION(oidnSetFilterImage);
  LOAD_FUNCTION(oidnSetSharedFilterImage);
  LOAD_FUNCTION(oidnSetFilterBool);
  LOAD_FUNCTION(oidnSetFilterInt);
  LOAD_FUNCTION(oidnSetFilterProgressMonitorFunction);
  LOAD_FUNCTION(oidnCommitFilter);
  LOAD_FUNCTION(oidnExecuteFilter);
  LOAD_FUNCTION(oidnGetDeviceError);
  LOAD_FUNCTION(oidnGetBufferData);
  LOAD_FUNCTION(oidnReadBuffer);
  LOAD_FUNCTION(oidnWriteBuffer);
  LOAD_FUNCTION(oidnReleaseFilter);
  LOAD_FUNCTION(oidnReleaseBuffer);
  LOAD_FUNCTION(oidnReleaseDevice);

#undef LOAD_FUNCTION

  return true;
};

// curl
//----------------------------------------------------------------------------//
static struct curl_t
{
  // NOLINTBEGIN
  using pfn_curl_easy_init = CURL* (*)(void);
  using pfn_curl_easy_setopt = CURLcode (*)(CURL* curl, CURLoption option, ...);
  using pfn_curl_easy_perform = CURLcode (*)(CURL* curl);
  using pfn_curl_easy_cleanup = void (*)(CURL* curl);

  using pfn_curl_slist_append = struct curl_slist* (*)(struct curl_slist* list,
                                                       const char* data);
  using pfn_curl_slist_free_all = void (*)(struct curl_slist* list);

  using pfn_curl_global_init = void (*)(long flags);
  using pfn_curl_global_cleanup = CURLcode (*)(void);

  pfn_curl_easy_init curl_easy_init{nullptr};
  pfn_curl_easy_setopt curl_easy_setopt{nullptr};
  pfn_curl_easy_perform curl_easy_perform{nullptr};
  pfn_curl_easy_cleanup curl_easy_cleanup{nullptr};

  pfn_curl_slist_append curl_slist_append{nullptr};
  pfn_curl_slist_free_all curl_slist_free_all{nullptr};

  pfn_curl_global_init curl_global_init{nullptr};
  pfn_curl_global_cleanup curl_global_cleanup{nullptr};
  // NOLINTEND

  void* curl{nullptr};
  bool init{false};
} curl;

//----------------------------------------------------------------------------//
static io_bool_t unload_curl(const io_logging_i* io_logging, bool error = false)
{
  if (error)
    common::log_message(io_logging, "Failed to load CURL library!");

  if (curl.curl)
    common::unload_library(curl.curl);

  memset(&curl, 0, sizeof(curl));
  return !error;
};

//----------------------------------------------------------------------------//
static io_bool_t load_curl(const io_logging_i* io_logging)
{
  if (curl.init)
    return true;

#ifdef _WIN32
  curl.curl = common::load_library("libcurl-x64");
#else
  curl.curl = common::load_library("libcurl.so.4");
#endif

  if (!curl.curl)
    return unload_curl(io_logging, true);

#define LOAD_FUNCTION(_function)                                               \
  curl._function =                                                             \
      (curl_t::pfn_##_function)common::load_function(curl.curl, #_function);   \
  if (!curl._function)                                                         \
    return unload_curl(io_logging, true);

  LOAD_FUNCTION(curl_easy_init);
  LOAD_FUNCTION(curl_easy_setopt);
  LOAD_FUNCTION(curl_easy_perform);
  LOAD_FUNCTION(curl_easy_cleanup);

  LOAD_FUNCTION(curl_slist_append);
  LOAD_FUNCTION(curl_slist_free_all);

  LOAD_FUNCTION(curl_global_init);
  LOAD_FUNCTION(curl_global_cleanup);

#undef LOAD_FUNCTION

  return true;
};

} // namespace libraries