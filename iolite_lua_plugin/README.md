# IOLITE Lua Plugin

Lua support in IOLITE is implemented via the C API and provided as a native plugin.

You can use the Lua plugin either as a starting point to roll your own custom scripting backend or to modify the existing Lua backend to your liking.

## Building the Plugin

Building the plugin requires a C++ compiler and a recent version of CMake.

The plugin can be built on any supported platform by running the `build_plugins.sh` shell script or by manually executing the following commands on the command line:

```bash
$ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
$ cmake --build build --config Release
```
