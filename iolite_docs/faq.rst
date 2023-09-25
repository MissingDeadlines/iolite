⁉️ Frequently Asked Questions
=============================

Here's a collection of frequently asked questions. Make sure to go through each one if you're encountering an issue. Chances are high that it is already known and documented here.

.. note:: If you do not find the answer you are looking for, head to our `Discord community <https://discord.com/invite/SZjfhw7z75>`_ and raise the question there.

"What are the overall system requirements?"
-------------------------------------------

**General Requirements**

- **System:** Windows 10+ or Linux
- **GPU:** Support for Vulkan 1.2 or higher
- **CPU:** Support for the AVX2 instruction set

**Optimal Performance**

- **GPU:** NVIDIA GTX 1080 (or equivalent) and higher

.. warning:: Please note that integrated graphics cards are not supported. Even if IOLITE starts, chances are high that the performance is far from optimal.

"Help, IOLITE crashes during startup!"
--------------------------------------

The chances are high that your GPU driver is outdated. Please use the most recent drivers from NVIDIA or AMD:

- | **Drivers for NVIDIA GPUs**
  | https://www.nvidia.com/download/index.aspx
- | **Drivers for AMD GPUs**
  | https://www.amd.com/en/support

If you're using Linux, using the latest proprietary driver is your best bet. Please ensure that your GPU driver is working correctly before trying to launch IOLITE.

If you're experiencing issues with the standalone Linux builds, it's potentially due to mismatches between the runtime libraries utilized by IOLITE and your Linux distribution. In this case, please use a build from our Flatpak repository, which ships with the correct versions of all the utilized runtime libraries. You can find details on how to set up the Flatpak builds at the top of this page.

"How the hell do I exit the game mode again!?"
----------------------------------------------

The default keybinding for opening the editor is `F3`. You can adjust this in the `keybindings_global.json` file.

"I only see a simple default scene; where are the samples?"
-----------------------------------------------------------

Head over to our `GitHub repository <https://github.com/MissingDeadlines/iolite>`_ and place the contents of the `iolite_samples` directory right next to the IOLITE executable, overwriting any existing files.

"I've installed the latest build via Flatpak but can't find where IOLITE stores its data!"
------------------------------------------------------------------------------------------

When launching IOLITE for the first time, it installs all its data in a directory named according to the current version in the following directory:

.. code-block:: console

  /.var/app/com.missing_deadlines.IOLITE/

Deleting this directory can be used to reset IOLITE to its defaults.

"Is it possible to use Lua auto-completion with IOLITE?"
--------------------------------------------------------

You can find IOLITE's Lua scripting API as a LUA file in the Lua API documentation. Drop it close to your script, and your favorite IDE should pick it up automatically.

If you are using Visual Studio Code, make sure to install the `Lua extension <https://marketplace.visualstudio.com/items?itemName=sumneko.lua>`_ available on the marketplace.

"I would love to try writing a native plugin using the C/C++ API. Do I need IOLITE PRO?"
----------------------------------------------------------------------------------------

The API header file is publicly available via our `public GitHub repository <https://github.com/MissingDeadlines/iolite>`_. In addition, the free version can also load native plugins, so it's possible to evaluate this feature before deciding to subscribe. Loading non-factory plugins in the free version will trigger a small watermark, though.
