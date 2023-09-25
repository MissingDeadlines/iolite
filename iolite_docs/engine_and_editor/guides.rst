Guides and Tutorials
====================

This section contains a collection of short guides and tutorials for IOLITE. Please note that all guides assume that the default data source is available, and you've got the default world loaded.

Writing a Hello World Script
----------------------------

This short tutorial serves as a step-by-step guide to writing your first Lua script in IOLITE.

Open up your favorite code editor and create a new file. Copy and paste the following Lua script, which logs two strings to the console:  

.. code-block:: lua

  Log.load()

  -- Logs each time the script gets (re-)loaded
  Log.log_info("Hello world! Script loaded!")

  function OnActivate(entity)
    -- Logs once the component becomes active
    Log.log_info("Hello world! Component active!")
  end

After that, continue with the following steps:

1. Store the script in `default/media/scripts/` and name it `hello_world.lua`
2. Open up IOLITE, ensure that the editor is active, and head over to the *World Inspector*
3. Create a new entity with a script component attached to it
4. In the property inspector, set the `Script` property to `hello_world` (without the extension)
5. Switch to the game mode by clicking `[Game Mode]` in the menu bar
6. Press `F2` to open up the console and check if the strings have been logged successfully

Keep IOLITE open and modify the strings passed to the log functions. Every time you save the script, it triggers a hot reload. Notice how the global log call gets executed while the call in `OnActivate` is not. This call can be, e.g., triggered by switching back and forth between the game mode and the editor; the editor can be activated using `F3`.

Loading a Flipbook Animation from MagicaVoxel
---------------------------------------------

In this tutorial we're going to load a flipbook animation authored in MagicaVoxel.

1. Create a flipbook animation in MagicaVoxel and add the VOX file to a data source, like, e.g., `default/media/voxels`. For testing purposes, you can also use one of the animated voxel assets that are shipped with MagicaVoxel, like the dinosaur or the deer
2. Create a voxel shape for your asset by dragging it from the *Prefab Browser* to the viewport
3. Attach a *Flipbook Animation* component to the entity you just created
4. Head to the entity inspector and set the `First Frame` and `Last Frame` properties to values matching your animation
5. Set the `Speed` property to a fitting value. 20 frames per second can be a good starting point
6. Enable the `Loop` and `Play` flags

If everything works out, your animation should be playing. The following video shows the depicted steps:

Writing Plugins Using the Native C/C++ API
------------------------------------------

IOLITE's C/C++ API offers the possibility to create highly modular engine and editor plugins in C, C++, or any other language that supports C bindings.

.. warning:: The API documentation is provided via the header file itself.
.. warning:: Native plugins are a feature of IOLITE PRO. You're free to evaluate plugins in the free version, but loading non-default plugins will add a watermark to the editor and game.

Writing a Simple Plugin
-----------------------

To write your first plugin, head over to our `public GitHub repository <(https://github.com/MissingDeadlines/iolite/tree/main/iolite_c_api>`__ and download the latest version of the API header file.

The header file is completely self-contained and should compile with any C or C++ compiler on any of the supported platforms. Simply set up a project that outputs a DLL and you are good to go. If you're unsure how to proceed, take a look at the [minimal sample plugin](https://github.com/MissingDeadlines/iolite/tree/main/iolite_c_api/sample_plugins) which shows how to use CMake to set up a plugin project for multiple platforms.

All a plugin has to do is provide the following three functions, as shown in the minimal plugin example in the repository:

.. code-block:: cpp

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

After compiling your plugin, create/modify the ``plugins.json`` file:

.. code-block:: json

  [
    {
      "name": "my_plugin",
      "filename": "MyPlugin"
    }
  ]

If everything worked out okay, you can fire up IOLITE and check the results via the console log output (``F2`` opens up the console) or the ``[Plugin Manager]`` available via the `[Plugins]` menu in the menu bar.

Writing a Custom Scripting Backend
----------------------------------

IOLITE's Lua integration is provided via a native plugin using the C API and the full implementation is available via our `public GitHub repository <https://github.com/MissingDeadlines/iolite/tree/main/iolite_lua_plugin>`_. If you're planning to roll your own custom scripting backend, the Lua implementation can serve as a great starting point.