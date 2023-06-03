# IOLITE - Voxel Game Engine

![IOLITE Logo](https://media.missing-deadlines.com/iolite/images/iolite_logo2.png)

A modern, portable game engine with an embedded editor. Completely scriptable in Lua. Free for personal and commercial use. More details, as well as current builds, can be found on the project's website: 

<https://iolite-engine.com>

This repository houses all public resources, like the [issue tracker](https://github.com/MissingDeadlines/iolite/issues), [auxiliary Python scripts](https://github.com/MissingDeadlines/iolite/tree/main/python_scripts), [various resources](https://github.com/MissingDeadlines/iolite/tree/main/various), and the [samples](https://github.com/MissingDeadlines/iolite/tree/main/iolite_samples).

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
$ # All data is stored in ~/.var/app/com.missing_deadlines.IOLITE/data/`
```

Flatpak builds come with all samples from this repository preinstalled.

## Engine Resources

- Details on how to use IOLITE are available in the [documentation](https://iolite-engine.com/docs)
- Changes are documented in the [changelog](https://iolite-engine.com/changelog)
- Planned features and changes are visible in the [roadmap](https://iolite-engine.com/roadmap)
- If you love IOLITE and like to support its development, take a look [here](https://iolite-engine.com/subscribe)

![Example scene rendered in IOLITE](media/screenshot.jpg?raw=true)
