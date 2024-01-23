.. _physics:

Physics
=======

This chapter serves as a guide for all the physics-based features available.

The role of voxel shapes
------------------------

Shapes, in addition to being the main visual asset, are also the central element of all physics simulations in IOLITE.

Voxel shapes can be used as static colliders or as dynamic or kinematic rigid bodies. A single shape can also be automatically split into different sub chunks based on the user interactions, as depicted in the section :ref:`destruction` below. They can also be used to create and simulate vehicles, like, e.g., cars or trucks, as depicted in :ref:`vehicle_physics`.

Character controllers
---------------------

Character controllers and specifically the *Character Controller* component can be used to implemented movement logic for NPCs or a character controllable by the player.

.. _destruction:

Destruction system
------------------
   
This section discusses the destruction subsystem.
   
Support structures
^^^^^^^^^^^^^^^^^^
   
To enable the destruction features for a shape, set the ``Support Structures`` *voxel shape flag* in the component. With this feature enabled, every time a shape is voxelized, the system scans the shape for islands/regions, classifying them using the adjacency of voxels. It then automatically creates new sub-shapes for the regions based on the result. The algorithm uses a 6-connectivity (the six faces of each voxel) for classifying the regions.

.. note:: Support structures also work with dynamic shapes marked as being kinematic. The logic and behavior in this case is the same as for static shapes.

The system also scans for regions connected to the ground for static shapes. A region is treated as having a ground connection when it contains a voxel in the bottom row of the shape (the y coordinate of the voxels equals zero). The resulting shape is kept static if a region has a ground connection. If a region loses its ground connection, the resulting shape will be set to dynamic.

Fracturing
^^^^^^^^^^

The following screenshot shows a wall split into chunks using IOLITE's fracturing system.

.. image:: /_static/images/fracturing.jpg
   
.. note:: To see the fracturing system in action, check out the *physics sample* in our repository.
   
Voxel connectivity
^^^^^^^^^^^^^^^^^^
   
Shapes can also be split based on the *voxel connectivity system*. This system allows you to specify the connectivity for each of the six faces of a voxel. This system, e.g., eases the implementation of scenarios where the player can cut shapes into pieces.

For example, if you want to cut a wall horizontally into two equally large pieces, call the ``disconnect`` voxel shape API function on all voxels in the center row, passing the flag for the top face. Doing this will disconnect all voxels from their top neighbors. The wall will be cut in half after calling ``voxelize`` for the shape.

.. _vehicle_physics:

Vehicle Physics
---------------

.. note:: Coming soon.