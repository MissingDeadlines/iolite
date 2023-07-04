# IOLITE - Voxel Game Engine

![IOLITE Logo](https://media.missing-deadlines.com/iolite/images/iolite_logo2.png)

A modern, portable game engine with an embedded editor. Completely scriptable in Lua. Free for personal and commercial use. More details, as well as current builds, can be found on the project's website:

<https://iolite-engine.com>

## Overview

This repository houses all public resources, like...

- [C/C++ API](https://github.com/MissingDeadlines/iolite/tree/main/iolite_c_api)  
Write native plugins in C or C++ or any other language that supports C bindings.
- [Lua plugin](https://github.com/MissingDeadlines/iolite/tree/main/iolite_lua_plugin)  
Completely decoupled from the engine core and implemented using the native API. Customize it to your liking or use it as a template for rolling your own backend.
- [Samples](https://github.com/MissingDeadlines/iolite/tree/main/iolite_samples)  
Kickstart your journey with IOLITE.
- [Issue tracker](https://github.com/MissingDeadlines/iolite/issues)  
Report bugs or request new features.
- [Auxiliary Python scripts](https://github.com/MissingDeadlines/iolite/tree/main/python_scripts)  
Generate packages for your IOLITE projects and more.
- [Various other resources](https://github.com/MissingDeadlines/iolite/tree/main/various)  
Contains additional resources like the display LUTs for color grading in DaVinci Resolve


## Join our Community

Join our [Discord community](https://discord.com/invite/SZjfhw7z75), follow the development on [Mastodon](https://mastodon.missing-deadlines.com/@benjamin), and become a part of our development journey.

## Download the Latest Version

- Download the [latest Windows build](https://iolite-engine.com/api/download_windows)
- Download the [latest Linux build](https://iolite-engine.com/api/download_linux) (tested on Ubuntu 22.04 LTS and the latest SteamOS)

If you'd like to receive automatic updates, or if you're using another Linux distribution, give our Flatpak builds a try:

```shell
$ flatpak --user --no-gpg-verify remote-add md https://media.missing-deadlines.com/iolite/flatpak
$ flatpak --user install md com.missing_deadlines.IOLITE
$ flatpak run com.missing_deadlines.IOLITE
$ # All data is stored in ~/.var/app/com.missing_deadlines.IOLITE/data/
```

Flatpak builds come with all samples from this repository preinstalled.

## Support this project with IOLITE PRO

IOLITE is available in two distinct flavors. The free version, which comes with the main set of features, and **IOLITE PRO**, which adds the following and supports the further development of IOLITE:

- **Native C/C++ API**  
Write highly modular plugins in C, C++, or in any other language that supports C bindings. *Please note that native plugins can also be evaluated in the free version*.
- **Powerful triangle mesh voxelization algorithm**  
Turn complex meshes with materials and textures exported from any 3D software that supports glTF, like Blender, 3ds Max, etc. or asset libraries like Quixel Megascans and PlantCatalog into beautiful voxel assets.
- **Export path-traced renders**  
Export LDR/HDR renders in the highest quality using the EXR file format.
- **Option to disable the splash screen**
IOLITE PRO offers the option to disable the splash screen shown during startup.
- **DRM free**  
Like the free version, IOLITE PRO is shipped without any DRM. Every downloaded build is yours forever to keep.

You can find more details on the PRO version on our [website](https://iolite-engine.com/subscribe).

## Engine Resources

- Details on how to use IOLITE are available in the [documentation](https://iolite-engine.com/docs).
- Changes are documented in the [changelog](https://iolite-engine.com/changelog).
- Feature requests and bugs are maintained in the [issue tracker](https://github.com/MissingDeadlines/iolite/issues).
- If you love IOLITE and like to support its development, take a look [here](https://iolite-engine.com/subscribe).

![Example scene rendered in IOLITE](media/screenshot.jpg?raw=true)
