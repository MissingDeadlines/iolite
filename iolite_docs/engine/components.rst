Component glossary
==================

This section serves as a glossary for all the available core components.

.. note:: The properties of the components are directly documented in the editor and can be accessed by hovering over the property name in the entity inspector.

Animation Node
--------------

Animation nodes are used for tagging entity hierarchies in the context of the :ref:`animation_system`.

Camera
------

Cameras are used to render the world from certain positions. Every world contains a ``editing_camera``, which is used in the editor, and a ``game_camera``, which is used while the game mode is active, by default. It's possible to create and activate additional cameras, either via the editor or via the scripting interface.

Camera Controller
-----------------

Camera controllers, as the name suggests, control the camera they are attached to. They can be used to create FPS- or third-person-style camera behaviors controllable via the scripting interface with little effort.

Character Controller
--------------------

Character controllers are used to provide physics-interactions for characters. The component provides a scripting interface for moving the characters.

Custom Data
-----------

*Custom Data* components offer the possibility to store custom properties for an entity. Properties can be read, added, modified, and removed via the editor or the scripting API.

Flipbook Animation
------------------

Use this component to animate voxel shapes using flipbook-style animations. Flipbook animations operate by changing the voxel data of the corresponding shape component on a frame by frame basis.

Importing a flipbook animation from MagicaVoxel
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In this short tutorial, we will load a flipbook animation authored in MagicaVoxel.

1. Create a flipbook animation in MagicaVoxel and add the VOX file to a data source (e.g., ``default/media/voxels``). For testing purposes, you can also use one of the animated voxel assets that are shipped with MagicaVoxel, like the dinosaur or the deer
2. Create a voxel shape for your asset by dragging it from the *Prefab Browser* to the viewport
3. Attach a *Flipbook Animation* component to the entity you just created
4. Head to the entity inspector and set the ``First Frame`` and ``Last Frame`` properties to values matching your animation
5. Set the ``Speed`` property to a fitting value. 20 frames per second can be a good starting point
6. Enable the ``Loop`` and ``Play`` flags

If everything works out, your animation should be playing.

Joint
-----

Joints can be used to connect voxel shapes with varying constraints. The component offers various types of joints to create various physics-based effects.

Kill Plane
----------

Kill planes can be used to automatically destroy voxel shapes which drop below the y-position of the node the component is attached to.

To ensure that the whole shape is below the plane before it is destroyed, the shape's bounding sphere is used. The logic is implemented as follows:

.. code-block:: c

  if (shape_bounds_center.y + shape_bounds_radius < kill_plane_position.y) {
    despawn_voxel_shape();
  }

Light
-----

Use this component to add lights to the scene. More details can be found in the chapter :ref:`lighting_and_gi`.

Mesh 
----

Mesh components are used to render triangle meshes with different materials. They are mainly used to render the sky sphere and water planes with the according materials.

Additional meshes can be provided by adding OBJ files to the ``media/meshes`` directory in any data source. Currently, only materials named ``default``, ``water``, or ``sky`` are supported.

Node
----

Node components are attached to every entity by default and provide a 3D-transformation (i.e., a position, orientation, and size). They also make it possible to create hierarchies of entities.

Post Effect Volume
------------------

Post effect volumes are used to add post effects to certain areas within the world. They are applied hierarchically based on their priority.

.. _sound_component:

Sound 
-----

Use this component to play sound effects at certain positions in the world.

Script
------

This component can be used to add scripts to the world.

Tag
---

Can be used to add tags to entities. Tags are useful for quickly looking up entities in the world via the APIs.

Text
----

Use this component to render text at different positions in the world. It's also useful for adding notes in the editor.

.. _component_voxel_shape:

Vehicle
-------

Vehicle components are used in the context of the :ref:`vehicle_physics` and form the base part of a vehicle.

Vehicle Wheel
-------------

Vehicle wheel components are used in the context of the :ref:`vehicle_physics` and define the wheel of a vehicle.

Voxel Shape
-----------

Voxel shapes are in charge of rendering and simulating everything voxel-related. Shapes are either initialized from authored VOX files available in the data sources, or they are procedurally filled with content (e.g., terrain using a script).

In addition, voxel shapes support the generation of support structures and fracturing for physics-based destruction effects.
