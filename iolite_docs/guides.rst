👀 Guides and tutorials
========================

This section contains a collection of short guides and tutorials for IOLITE. All guides assume the default data source is available, and that you have loaded the default ``basic`` world.

Writing your first Hello World script in Lua
--------------------------------------------

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
-----------------------------------------------

In this short tutorial, we will load a flipbook animation authored in MagicaVoxel.

1. Create a flipbook animation in MagicaVoxel and add the VOX file to a data source, like, e.g., ``default/media/voxels``. For testing purposes, you can also use one of the animated voxel assets that are shipped with MagicaVoxel, like the dinosaur or the deer
2. Create a voxel shape for your asset by dragging it from the *Prefab Browser* to the viewport
3. Attach a *Flipbook Animation* component to the entity you just created
4. Head to the entity inspector and set the ``First Frame`` and ``Last Frame`` properties to values matching your animation
5. Set the ``Speed`` property to a fitting value. 20 frames per second can be a good starting point
6. Enable the ``Loop`` and ``Play`` flags

If everything works out, your animation should be playing.