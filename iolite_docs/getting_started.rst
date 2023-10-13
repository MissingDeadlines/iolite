👀 Getting Started
===================

This section contains installation instructions and guides to get you started.

Installing portable builds (Windows and Linux)
----------------------------------------------

Installing IOLITE is trivial. Simply download a release from the `landing page <https://iolite-engine.com>`_ or, if you have subscribed to IOLITE PRO, from the `subscriber area <https://iolite-engine.com/subscribers>`_ and unzip it. Finally, launch the IOLITE executable. That's it. 🙂 

If you encounter an issue when running IOLITE, please consult the section :ref:`faq`. If you are unable to solve the issue, please follow the steps depicted in :ref:`reporting_issues`.

Installing Flatpak builds (Linux)
---------------------------------

In addition to the pre-packaged builds, IOLITE is available via our Flatpak repository. Flatpak builds provide automatic updates and also work on more uncommon Linux distributions. To add our repository as a remote, install IOLITE, and launch it, please perform the following steps:

.. code-block:: bash

  $ flatpak --user --no-gpg-verify remote-add md https://media.missing-deadlines.com/iolite/flatpak
  $ flatpak --user install md com.missing_deadlines.IOLITE
  $ flatpak run com.missing_deadlines.IOLITE
  $ # All data is stored in ~/.var/app/com.missing_deadlines.IOLITE/data/
  
Installing also adds a shortcut to IOLITE to your favorite launcher. After that, you can update IOLITE like any other Flatpak application:

.. code-block:: bash

  $ flatpak --user update com.missing_deadlines.IOLITE

In contrast to the portable builds, the Flatpak builds are also shipped with all the latest samples from the GitHub repository.

Beta releases (PRO only)
------------------------

Beta releases are released in between main releases for PRO subscribers. Please consider the following when working with a beta release of IOLITE:

.. important:: Beta releases are released more frequently and contain the latest features, but they can be a little less stable.

- Please switch to the ``develop`` branch in our public GitHub repository and search for a tag matching the version of IOLITE you are using
- Please ensure that you are using a matching version of the open-source plugins. Due to potential API changes, there are separate plugin releases available for the beta versions of IOLITE
  
Telemetry (Usage statistics)
----------------------------

IOLITE, by default, gathers some elementary **anonymous usage statistics** to aid the development.

.. note:: This system will be extended in the future also to report crashes.

The following data is collected for each of the logged events:

UUID
   A unique identifier based on a **hash** of the addresses of **all** the network adapters installed in your system, e.g., ``8A36-E187-2C1F-51FB``. This hash **can not be** reversed to get access to the MAC addresses of your adapters
Platform
   Either ``Windows`` or ``Linux``
Build
  Either ``Free`` or ``Pro``
API version
  The version of the API, e.g., ``0.4.0``
Event name
  The event's name, e.g., ``startup``, ``shutdown``, etc.
Event metadata
  Currently unused and reserved for future use
  
You can turn off the telemetry system at any time by adding the following member to your ``app_metadata.json`` file:

.. code-block:: json
   
  "disable_telemetry": true

Guides
------

This section contains a collection of short guides and tutorials for IOLITE. All guides assume the default data source is available, and that you have loaded the default ``basic`` world.

Writing your first Hello World script in Lua
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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

1. Store the script in ``default/media/scripts/`` and name it ``hello_world.lua``
2. Open up IOLITE, ensure that the editor is active, and head over to the *World Inspector*
3. Create a new entity with a script component attached to it
4. In the property inspector, set the ``Script`` property to ``hello_world`` (without the extension)
5. Switch to the game mode by clicking ``[Game Mode]`` in the menu bar
6. Press ``[F2]`` to open up the console and check if the strings have been logged successfully

Keep IOLITE open and modify the strings passed to the log functions. Every time you save the script, it triggers a hot reload. Notice how the global log call gets executed while the call in ``OnActivate`` is not. This call can be, e.g., triggered by switching back and forth between the game mode and the editor; the editor can be activated using ``[F3]``.

Importing a flipbook animation from MagicaVoxel
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In this short tutorial, we will load a flipbook animation authored in MagicaVoxel.

1. Create a flipbook animation in MagicaVoxel and add the VOX file to a data source, like, e.g., ``default/media/voxels``. For testing purposes, you can also use one of the animated voxel assets that are shipped with MagicaVoxel, like the dinosaur or the deer
2. Create a voxel shape for your asset by dragging it from the *Prefab Browser* to the viewport
3. Attach a *Flipbook Animation* component to the entity you just created
4. Head to the entity inspector and set the ``First Frame`` and ``Last Frame`` properties to values matching your animation
5. Set the ``Speed`` property to a fitting value. 20 frames per second can be a good starting point
6. Enable the ``Loop`` and ``Play`` flags

If everything works out, your animation should be playing.