# IOLITE C API

IOLITE's C API allows you to write native engine and editor plugins. The API itself is documented in the `iolite_api.h` header file.

IOLITE's API header file is completely self-contained and dependency-free. Compiling a plugin DLL should *just work* given any C or C++ compiler on any supported platform. More details can be found in the [official documentation](https://iolite-engine.com/docs).

To get started, there is a small selection of sample plugins available in the `sample_plugins` folder. For a more advanced example, you can also check out the Lua plugin available in the root directory of this repository.

## Building the Sample Plugins

Building the sample plugins requires a C/C++ compiler and a recent version of CMake.

The plugins can be built on any supported platform by running the `build_plugins.sh` shell script or by manually executing the following commands on the command line:

```bash
$ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
$ cmake --build build --config Release
```

## Minimal C/C++ Plugin

Here's a minimal plugin you can build and check if it can be loaded in IOLITE:

```c++
#include "iolite_api.h"

const struct io_api_manager_i* io_api_manager = 0;

IO_API_EXPORT io_uint32_t IO_API_CALL get_api_version()
{
  // Inform IOLITE which version of the API you are using
  return IO_API_VERSION;
}

IO_API_EXPORT io_int32_t IO_API_CALL load_plugin(const void* api_manager)
{
  // Ensure we can keep accessing the API manager after loading the plugin
  io_api_manager = (const struct io_api_manager_i*)api_manager;

  // Do something with the API manager, set up your plugin, etc.

  return 0; // Return a value < 0 to indicate that the loading of your plugin
            // has failed (depedency not available, etc.)
}

IO_API_EXPORT void IO_API_CALL unload_plugin()
{
  // Clean up here
}
```

To load the plugin, don't forget to add the following member to the `app_metadata.json` file of your project:

```json
"plugins": [
  [
    "a_descriptive_name_for_my_plugin",
    "PluginFilenameWithoutExtension"
  ]
]
```