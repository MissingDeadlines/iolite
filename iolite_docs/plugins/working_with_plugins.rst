Working with plugins
====================

This section serves as a brief introduction how plugins are handled in IOLITE and how to install or compile IOLITE's open source plugins.
 
Loading plugins
---------------

The compiled plugin libraries (DLL or SO files) have to be placed in IOLITE's root directory, right next to the executable.

In addition, the engine sources the plugins which should be loaded from the optional ``plugins.json`` file in the root directory. The file requires the following structure:

.. code-block:: json

  [
    {
      // A descriptive name for your plugin
      "name": "my_plugin",
      // The filename *without* the extension
      "filename": "MyPlugin",
      // Set this to "false" to require loading the plugin manually (Optional)
      "load_on_startup": true
      // Used by the free version of IOLITE to whitelist the open source plugins (Optional)
      "checksum": "ABC"
    },
    {
      "name": "another_plugin",
      // ...
    }
  ]
  
Installing the open source plugins
----------------------------------

The plugins documented in this chapter are available as open source plugins in our public GitHub repository. The C/C++ sources of the plugins can be found here:

https://github.com/MissingDeadlines/iolite/tree/main/iolite_plugins

Even though compiling the plugins is straightforward, the plugins are also available as binaries for Windows and Linux, made available using GitHub releases. You can download a release matching your version of IOLITE here:

https://github.com/MissingDeadlines/iolite/releases

To load all the available, simply extract the contents of the ``windows`` or ``linux`` subdirectories in the ``plugins.zip`` file in the root directory of IOLITE, and you are good to go.

.. important:: There is no need to create the ``plugins.json`` file manually. Each release ships with a ``plugins.json`` file containing all the available open source plugins.
   
.. warning:: The plugin libraries have to be placed **right next** to the executable. Placing them in a subdirectory **will not** work.
 
Compiling the sample and open source plugins
--------------------------------------------

Building the plugins requires a C++ compiler and a recent version of CMake.

The plugins can be built on any supported platform by either running `build_plugins.bat/sh` or by manually executing the following commands on the command line:

.. code-block:: bash

  $ cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
  $ cmake --build build --config Releas