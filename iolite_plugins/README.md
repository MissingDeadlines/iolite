# Factory Plugins

This directory houses all the available factory plugins.

## Building the Plugins

Building the plugins requires a C++ compiler and a recent version of CMake.

The plugins can be built on any supported platform by either running `build_plugins.bat/sh` or by manually executing the following commands on the command line:

```bash
$ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
$ cmake --build build --config Release
```
