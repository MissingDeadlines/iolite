.. _writing plugins:

Writing plugins using the native C/C++ API
==========================================

IOLITE's C/C++ API offers the possibility to create highly modular engine and editor plugins in C, C++, or any other language that supports C bindings.

.. important:: The API documentation is provided via the header file itself.

.. important:: Native plugins are a feature of IOLITE PRO. You're free to evaluate plugins in the free version, but loading non-default plugins will add a watermark to the editor and game.

Writing a simple plugin
^^^^^^^^^^^^^^^^^^^^^^^

To write your first plugin, head over to our `GitHub repository <https://github.com/MissingDeadlines/iolite/tree/main/iolite_c_api>`_ and download the latest version of the API header file.

The header file is completely self-contained and should compile with any C or C++ compiler on any of the supported platforms. Simply set up a project that outputs a DLL and you are good to go. If you're unsure how to proceed, take a look at the `minimal sample plugin <https://github.com/MissingDeadlines/iolite/tree/main/iolite_c_api/sample_plugins>`_ which shows how to use CMake to set up a plugin project for multiple platforms.

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

After compiling your plugin, create/modify the ``plugins.json`` file in the root directory of the application. The structure of the file is depicted in the section :ref:`loading_plugins`.

If everything worked out okay, you can fire up IOLITE and check the results via the console log output (``[F2]`` opens up the console) or the ``[Plugin Manager]`` available via the ``[Plugins]`` menu in the menu bar.

Writing a custom scripting backend
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

IOLITE's Lua integration is provided via a native plugin using the C API, and the full implementation is available via our `public GitHub repository <https://github.com/MissingDeadlines/iolite/tree/main/iolite_lua_plugin>`_. The Lua implementation can be a great starting point if you plan to roll your own custom scripting backend.