# IOLITE - Voxel Game Engine

![IOLITE Logo](https://media.missing-deadlines.com/iolite/images/iolite_logo2.png)

A highly flexible, voxel-powered game engine with an embedded editor. Create games using Lua, the native C/C++ API, and assets authored in voxel editors like MagicaVoxel or Avoyd.

<https://iolite-engine.com>

## Overview

This repository houses all public resources, like...

- [C/C++ API](iolite_c_api)  
Write native plugins in C or C++ or any other language that supports C bindings.
- [Factory Plugins](iolite_plugins)  
IOLITE's open factory plugins, including the heightmap-based terrain generator and the Lua scripting backend. Fully customizable and great references for rolling custom implementations.
- [Samples](iolite_samples)  
Kickstart your journey with IOLITE.
- [Issue tracker](https://github.com/MissingDeadlines/iolite/issues)  
Report bugs or request new features.
- [Auxiliary Python scripts](python_scripts)  
Generate packages for your IOLITE projects and more.
- [Various other resources](various)  
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

## Third-Party Plugins

If you've developed a custom plugin for IOLITE, make sure to send us a message, and we'll add a link in this section.

## Engine Resources

- Details on how to use IOLITE are available in the [documentation](https://iolite-engine.com/docs).
- Changes are documented in the [changelog](https://iolite-engine.com/changelog).
- Feature requests and bugs are maintained in the [issue tracker](https://github.com/MissingDeadlines/iolite/issues).
- If you love IOLITE and like to support its development, take a look [here](https://iolite-engine.com/subscribe).

![Example scene rendered in IOLITE](media/screenshot.jpg?raw=true)
