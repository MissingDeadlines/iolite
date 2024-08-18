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

#include "iolite_api.h"

#ifndef INCLUDE_IO_PLUGIN_LUA_API
#define INCLUDE_IO_PLUGIN_LUA_API

// Inteface exposed by the Lua scripting backend plugin
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
#define IO_PLUGIN_LUA_USER_CALLBACK_API_NAME "io_plugin_lua_user_callback_i"
//----------------------------------------------------------------------------//

struct io_plugin_lua_user_callack_i // NOLINT
{
  // Called when the Lua state for a script should be initialized. Use this
  // function to expose functionality from other plugins.
  void (*on_init_lua_state)(void* lua_state);
};

//----------------------------------------------------------------------------//
#define IO_PLUGIN_LUA_API_NAME "io_plugin_lua_i"
//----------------------------------------------------------------------------//

struct io_plugin_lua_i // NOLINT
{
  // Executes the provided lua script
  void (*execute_script)(const char* script);
};

#endif