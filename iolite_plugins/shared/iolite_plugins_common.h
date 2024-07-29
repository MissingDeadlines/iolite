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

// API
#include "iolite_api.h"

// Dependencies
#include <string>
#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#undef min
#undef max
#else
#include <dlfcn.h>
#endif

#ifndef _IO_PLUGIN_NAME
#define _IO_PLUGIN_NAME "Unknown"
#endif

//----------------------------------------------------------------------------//
namespace common
{

//----------------------------------------------------------------------------//
void* load_library(const char* name)
{
#ifdef _WIN32
  return LoadLibraryA(name);
#else
  void * handle = dlopen(name, RTLD_LAZY);
  if (!handle)
    handle = dlopen((std::string("lib") + name).c_str(), RTLD_LAZY);
  if (!handle)
    handle = dlopen((std::string(name) + ".so").c_str(), RTLD_LAZY);
  if (!handle)
    handle = dlopen((std::string("lib") + name + ".so").c_str(), RTLD_LAZY);
  return handle;
#endif
}

//----------------------------------------------------------------------------//
void* load_function(void* handle, const char* name)
{
#ifdef _WIN32
  return (void*)GetProcAddress((HMODULE)handle, name);
#else
  return dlsym(handle, name);
#endif
}

//----------------------------------------------------------------------------//
void unload_library(void* handle)
{
#ifdef _WIN32
  FreeLibrary((HMODULE)handle);
#else
  dlclose(handle);
#endif
}

//----------------------------------------------------------------------------//
inline static void log_message(const io_logging_i* io_logging, const char* fmt,
                               ...)
{
  char buffer[256];

  va_list args;
  va_start(args, fmt);
  stbsp_vsnprintf(buffer, 256, fmt, args);
  va_end(args);

  io_logging->log_plugin(_IO_PLUGIN_NAME, buffer);
}

//----------------------------------------------------------------------------//
} // namespace common