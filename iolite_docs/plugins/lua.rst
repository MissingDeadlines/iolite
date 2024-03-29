.. _plugin_lua:

🌝 Lua Scripting
=================

IOLITE uses Lua to provide its scripting functionality. Lua is an excellent language for scripting due to its minimal footprint, efficiency, and ease of use.

If you have not worked with Lua before, make sure to check out the following resources to learn the language:

Programming in Lua (First Edition)
   https://www.lua.org/pil/contents.html
The Lua 5.1 Reference Manual
   https://www.lua.org/manual/5.1/

.. important:: This section serves as an introduction to the Lua scripting integration in IOLITE. For further details, make sure to check out the :ref:`api_lua` documentation to quickly locate the functionality you are looking for.

Writing your first script in Lua
--------------------------------

This short tutorial serves as a step-by-step guide to writing your first Lua script in IOLITE.

.. important::
  This guide requires the latest version of the Lua plugin for IOLITE to be installed. Please check the section :ref:`working_with_plugins` for more details.

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

Lua runtime and libraries
-------------------------

IOLITE uses ``LuaJIT-2.1.0-beta3`` for maximum performance, which supports the language features of Lua 5.1. The following standard Lua libraries are available:

- base, coroutine, string, and table

Check out the following resources if you're interested in the features and implications induced by using the LuaJIT runtime:

LuaJIT Homepage
   https://luajit.org
LuaJIT Overview
   https://luajit.org/luajit.html
LuaJIT Benchmarks
   https://luajit.org/performance_x86.html

The basic structure of scripts
------------------------------

When using scripts, IOLITE expects you to provide a particular set of callback functions with the correct naming and parameters, which it can call in different scenarios.

.. important:: Please note that you do not need to provide all callback functions, even not providing any callback function is fine. IOLITE checks for the availability of each upfront.

The ``minimal.lua`` script available in the ``default`` data source contains stubs for all the available callback functions and can serve as a template for creating new scripts.

Let's have a closer look at each of the available callback functions.

Available callback functions
----------------------------

.. code-block:: lua

  function OnActivate(entity)
  end

Called precisely once during a script's lifetime.

This function is called once when the script becomes active. Scripts become active when, e.g., a world is loaded or if an entity with an attached script component gets spawned. This is the right place to set up additional resources and variables for your script.

.. code-block:: lua

  function OnDeactivate(entity)
  end

Called precisely once during a script's lifetime.

This is the counterpart to ``onActivate`` and is called once when the script becomes active, either by unloading a world or by destroying a script component during runtime. Use this to tear down additional resources created by your script.

.. code-block:: lua

  function Tick(entity, delta_t)
  end

Called precisely once each rendered frame. The delta time equals the time that has passed since the last call to this function.

Use this function for functionality that has a visual effect, like updating the final position of a character or a projectile, for example. It's also the right spot to react to the user's input as quickly as possible.

In general, it's wise to keep the workload in this function to a minimum and, e.g., implement actual gameplay and AI logic in the ``OnUpdate`` callback function at a lower frequency. The results computed at the lower frequency can then be interpolated in this function to achieve visually pleasing results.


.. code-block:: lua

  function TickPhysics(entity, delta_t)
  end

Called zero or multiple times per frame. The delta time equals the *fixed* delta time used for stepping the physics simulation.

This function is executed in lockstep with the physics simulation. Use this function to implement functionality that interacts with the physics simulation. Don't use this to modify the visual state, or you'll quickly run into visual stutter.

.. code-block:: lua

  function Update(entity, delta_t)
  end

Called each time the update interval specified in the script component has passed. The delta time equals the specified update interval.

Use this callback for implementing logic that has no imminent visual effect. This is the perfect spot for implementing AI and gameplay logic.

.. important:: Don't use this function for reacting on input or for updating data that has a visual effect!

.. code-block:: lua

  function OnEvent(entity, events)
  end

Called as soon as one or multiple events are available.

All the different types of available events are described in a later section. But the grasp the general concept, here's an example of handling contact events that occur when voxel shapes, and their rigid bodies, interact with each other:

.. code-block:: lua

  function OnEvent(entity, events)
      -- Iterate over all the available events
      for i = 1, #events do
        local e = events[i]
        -- Handle contact events
        if e.type == "Contact" then
          -- Provides the position of the contact
          -- "e.data.pos", the resulting impulse "e.data.impulse", 
          -- and the interacting entities "e.data.entity0"
          -- and "e.data.entity1"
        end
      end

A similar callback function is available for user events:

.. code-block:: lua

  function OnUserEvent(entity, events)
  end

Called as soon as one or multiple user events are available.

This callback function is useful for inter-script communication and data sharing. Listening to and sending of user events can be controlled via the ``Events`` interface table.

Loading API interfaces
----------------------

IOLITE provides a lot of different API interfaces for all the available subsystems. To ensure that scripts have a minimal footprint, you have to explicitly state which interfaces you want to use at the beginning of your script.

As an example, if you want to work with nodes and print some text to the log/console, you'll have to load the ``Log`` and ``Node`` interface tables like this:

.. code-block:: lua

  Node.load()
  Log.load()

In this example, the calls to ``load()`` populate the functions provided by the interfaces ``Node`` and ``Log`` via the according global tables.

Please note that not loading the API interfaces will lead to errors stating that the requested function is unavailable.

Hot reloading and error logging
-------------------------------

Scripts are hot-reloaded on every change you make. Potential errors and your log calls end up in IOLITE's console and log file. To toggle the console, press ``[F2]``.

If executing the script throws an error, go ahead and adjust the faulty line of code, save the file, and directly check back in IOLITE if the error is gone. It's as easy as that.

Date structures and refs
------------------------

When interacting with IOLITE via the scripting interface, you'll encounter three different types of data structures:

PODs (Plain Old Data)
   Vectors provided by the math interface, etc.
Refs
   Used to reference entities, components, and resources on engine-side
Handles
   Like refs, but specific to certain subsystems, like, e.g., the particle or sound system

Refs, compared to handles, are agnostic of the underlying subsystems. A ref can reference any component, entity, or resource, providing interfaces for checking the underlying type and whether the referenced resource is still alive.

Let's look at some examples of how refs can be utilized in detail. Here we're searching for a specific entity in the scene and checking whether it's available:

.. code-block:: lua

  Entity.load()

  -- Try to find the "goose" entity in the world
  local goose = Entity.find_first_entity_with_name("goose")
  if Ref.is_valid(goose) then
    -- Do something to the goose...
  end

Now we're dealing with a ref of unknown origin, and we want to make sure it is (A) a node and (B) still alive:

.. code-block:: lua

  Node.load()

  -- Check if a given ref is referencing a node component
  -- and whether the component is still alive
  if Node.get_type_id() == Ref.get_type_id(my_potential_node)
      and Node.is_alive(my_potential_node) then
    -- Retrieve the position when we're safe
    local pos = Node.get_world_position(my_potential_node)
    -- Do something with the position...
  end

Error handling and scripts
--------------------------

IOLITE strives for a good mixture of error handling and performance.

While a lot of user errors won't make the engine crash, like, e.g., passing the wrong amount of parameters to a function, there are certain cases where this behavior is expected, mostly related to interacting with resources and refs:

- Using the ref on an entity, component, or resource which is no longer alive. Make sure to only interact with alive resources using the ``is_alive`` function of the corresponding interface table
- Using an invalid ref to execute functions. Ensure you're always using valid refs using ``Ref.is_valid(ref_in_requestion)``
  
Going further
-------------

Our GitHub repository houses a couple of `Lua-based samples <https://github.com/MissingDeadlines/iolite/tree/main/iolite_samples>`_ which serve as an excellent reference and starting point. Otherwise, header over to the :ref:`api_lua` documentation to quickly locate the functionality you are looking for.