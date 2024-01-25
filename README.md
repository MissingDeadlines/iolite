# IOLITE - Voxel Game Engine

![IOLITE Logo](https://media.missing-deadlines.com/iolite/images/iolite_logo2.png)

[![Documentation Status](https://readthedocs.org/projects/iolite-voxel-game-engine/badge/?version=latest)](https://docs.iolite-engine.com/en/latest/?badge=latest)
[![build_plugins](https://github.com/MissingDeadlines/iolite/actions/workflows/build_plugins.yml/badge.svg)](https://github.com/MissingDeadlines/iolite/actions/workflows/build_plugins.yml)
[![build_sample_plugins](https://github.com/MissingDeadlines/iolite/actions/workflows/build_sample_plugins.yml/badge.svg)](https://github.com/MissingDeadlines/iolite/actions/workflows/build_sample_plugins.yml)

A highly flexible, voxel-powered game engine with an embedded editor. Create games using Lua, the native C/C++ API, and assets authored in voxel editors like MagicaVoxel or Avoyd.

More details can be found on our website: <https://iolite-engine.com>

![The forest sample scene has been created solely using the editor and the new voxel editing plugin. The characters were created using MagicaVoxel and can be directly used as assets.](media/screenshot.jpg?raw=true)

## Overview

This repository houses all public resources, including the following:

- [C/C++ API](iolite_c_api)  
Write native plugins in C or C++ or any other language that supports C bindings.
- [Open-source plugins](iolite_plugins)  
Including the voxel editor, terrain generator, and the Lua scripting backend. Fully customizable and great references for rolling custom implementations.
- [Samples](iolite_samples)  
A collection of Lua-based samples to kick-start your journey with IOLITE.
- [Issue tracker](https://github.com/MissingDeadlines/iolite/issues)  
Report bugs or request new features.
- [Auxiliary Python scripts](python_scripts)  
Generate packages for your IOLITE projects and more.
- [Additional resources](various)  
Various resources like the display lookup tables (LUTs) for color grading in DaVinci Resolve.

## Join our Community

Join our [Discord community](https://discord.com/invite/SZjfhw7z75), follow the development on [Mastodon](https://mastodon.missing-deadlines.com/@benjamin), and become a part of our development journey.

## Download

Download the latest binary distribution of IOLITE.

### Builds for Windows (Windows 10+)

- Download the [latest IOLITE build](https://iolite-engine.com/api/download_windows)
- Download the [latest IOLITE PRO build](https://iolite-engine.com/api/download_windows_pro)
- Download the [latest IOLITE PRO beta build](https://iolite-engine.com/api/download_windows_beta)

### Builds for Linux (Ubuntu 22.04, SteamOS, etc.)

- Download the [latest build](https://iolite-engine.com/api/download_linux)

## Support this project

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

## Third-Party Plugins

If you've developed a custom plugin for IOLITE, make sure to send us a message, and we'll add a link in this section.

## Engine Resources

- Details on how to use IOLITE are available in the [documentation](https://docs.iolite-engine.com).
- Changes are documented in the [changelog](https://iolite-engine.com/changelog).
- Feature requests and bugs are maintained in the [issue tracker](https://github.com/MissingDeadlines/iolite/issues).
- If you love IOLITE and like to support its development, take a look [here](https://iolite-engine.com/subscribe).
