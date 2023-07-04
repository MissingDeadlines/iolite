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

// Interfaces we use
//----------------------------------------------------------------------------//
const struct io_api_manager_i* io_api_manager;
const struct io_logging_i* io_logging;

// Interfaces we provide
//----------------------------------------------------------------------------//
struct io_user_task_i io_user_task;

//----------------------------------------------------------------------------//
void tick(float delta_t)
{
  static float time_passed = 0.0f;
  time_passed += delta_t;

  // Log a message every second
  if (time_passed >= 1.0f)
  {
    io_logging->log_info("Hello from your C plugin!");
    time_passed = 0.0f;
  }
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

  // Set up and register user task 
  memset(&io_user_task, 0, sizeof(io_user_task));
  io_user_task.on_tick = tick;
  io_api_manager->register_api(IO_USER_TASK_API_NAME, &io_user_task);

  return 0;
}

//----------------------------------------------------------------------------//
IO_API_EXPORT void IO_API_CALL unload_plugin()
{
  // Unregister our user task
  io_api_manager->unregister_api(&io_user_task);
}
