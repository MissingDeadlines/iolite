Basic usage of the editor
=========================

This section introduces the editor and its functionality.

Compared to other game engines, the editor is integrated into the runtime. An integrated editor has the tremendous advantage of being usable at any time to, e.g., directly inspect gameplay scenarios while testing your game. It also allows shipping the same toolset for modding purposes without additional effort.

Moving and rotating the camera
------------------------------

By default, the editor starts with the *Free Camera* enabled.

The *Free Camera* can be moved forward, left, back, and right relative to the current viewing direction using the ``[W]``, ``[A]``, ``[S]`` and ``[D]`` keys. It can be moved up and down using ``[E]`` and ``[Q]``. To rotate the camera, right-click, hold, and drag in the viewport.

In addition, IOLITE supports an *Orbit Camera*. To activate it, click ``[View]`` in the main menu bar and select ``[Orbit Camera]`` from the dropdown list under *Camera*.

With the *Orbit Camera* active, right-click, hold, and drag in the viewport to rotate the camera around the current center position. To move the camera, center-click, hold, and drag. To zoom in and out, use the mouse wheel.

Running your game
-----------------

The engine core utilizes the concepts of game states to provide different isolated feature sets to its users.

There are two of them available:

- While pausing some underlying subsystems like, e.g., the physics simulation and scripts, the *editing game state* houses IOLITE's editor.
- In contrast, the *main game state* ticks all gameplay-related subsystems and provides no editing functionality. This is state that is active when you want to test or ship your game.

.. important::

   When launching IOLITE in its default configuration, the application starts in the *editing game state*. How to make the *main game state* the default after launch is described in :ref:`app_metadata`.

To switch to the *main game state* in the editor, click on ``[Game Mode]`` in the menubar of the editor. When clicking this button, IOLITE stores a snapshot of the current world, switches the active camera from the ``editing_camera`` to the ``game_camera``, and activate the *main game state*. By pressing ``[F3]`` while the *main game state* is active, the *main game state* gets deactivated, the previous state of the world gets restored, and the editor becomes active again.

Working with snapshots
----------------------

When entering the *main game state*, IOLITE automatically generates a snapshot of the current state of the world. When returning to the editor, the previous state is automatically restored, discarding all changes that occurred while the *main game state* was active.

It might be helpful to skip restoring the state, e.g., to inspect a particular gameplay scenario. Accordingly, it is possible to skip restoring the state automatically by holding ``[CTRL]`` while clicking on the ``[Game Mode]`` item in the main menu bar. Back in the editor, the snapshot can then be manually restored by clicking on the *red load icon*.

In addition, it is possible to create and restore snapshots manually. To create a snapshot, click on the *green save icon*. Snapshots are stored as a stack, making it possible to store multiple snapshots and restore them in the exact order they were created. To restore the latest snapshot, click in the *red load icon*, as mentioned before.


Working with entities and components
------------------------------------

As teased in :ref:`ecs`, entities and components form the basis for creating games with IOLITE. This section covers some basics, like, e.g., creating and removing entities in the editor, how to clone entities, and how entities can be decorated with components.

Selecting entities
^^^^^^^^^^^^^^^^^^

Select entities either by clicking on them in the *World Inspector* or visually by clicking on them in the viewport. Visual picking only works if the selection tool is active in the editing toolbar.

Moving and rotating entities
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Select the entity you want to transform and select either the transform, rotate, or the combined gizmo from the editing toolbar. Click, hold, and drag the gizmo handles to apply the desired transformation.

Creating and removing entities
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To create a new entity in the editor, right-click on the entity in the *World Inspector* you want to attach the new entity to. In the context menu, either select ``[Attach New Entity]`` or ``[Attach New Entity (With Component)]`` to create an entity decorated with the selected component from the start.

Remove entities by selecting ``[Remove Entity]`` from the context menu or pressing ``[Del]`` after selecting it in the *World Inspector*.

Adding and removing components
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To add a component to an existing entity, right-click on it in the *World Inspector*, select ``[Add Component]`` in the context menu, and select the component you'd like to add.

To remove a component, select ``[Remove Component]`` in the context menu, and select the name of the component you want to remove.

Cloning entities
^^^^^^^^^^^^^^^^

Clone entities by selecting them and choosing ``[Clone Entity]`` from the context menu. It's also possible to clone entities by pressing ``[Ctrl + D]``.

.. important:: Cloning entities also copies the whole node hierarchy and all the attached components.

Renaming entities
^^^^^^^^^^^^^^^^^

Select the entity you want to rename and select ``[Rename Entity]`` from the context menu. Enter the new name in the text field and finalize the operation by clicking ``[Rename]``.

Hiding entities
^^^^^^^^^^^^^^^

It's possible to hide entities in the scene by clicking on the eye icon shown right next to them in the World Inspector. Hidden entities are tagged with a crossed-out eye icon. Hiding an entity also implicitly hides all of its descendants. Entities that are hidden implicitly by one of their ancestors are shown with a greyed-out eye icon or a grayed-out crossed one if they're also hidden explicitly.

Parenting entities
^^^^^^^^^^^^^^^^^^

Changing the hierarchy of entities is possible by dragging and dropping in the *World Inspector*. To parent an entity with another one, drag and drop it onto the desired parent entity. To unparent an entity, drag it onto the root entity of the world.

Editing worlds
--------------

This section introduces various useful features for quickly editing worlds in IOLITE.

Creating, saving, and loading worlds
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- To create a new world, open up the ``[File]`` menu and select ``[Create New World]``.
- To load an existing world, hover over ``[Load World]`` in the ``[File]`` menu and select the desired world.
- To save the current world, select ``[File] => [Save World]``.
- To save the current world *to a separate file*, select ``[File] => [Save World As...]``
- To reload the current world from the disk, select ``[File] => [Reload World]``

Adding voxel shapes to the world
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To quickly position a voxel shape in the world, head over to the *Prefab Browser*. Click and hold the desired shape and drag it into the viewport. Release the mouse button when you're happy with the position.

Another option is to manually create an entity with an attached *Voxel Shape* component and pick the desired shape via the component's properties.

Saving and loading prefabs
^^^^^^^^^^^^^^^^^^^^^^^^^^

Prefabs are hierarchies of entities that can be saved to and loaded from disk. They can either be used to ease editing or to create modules, like, e.g., a character that gets spawned using a script.

To create a prefab, select the root entity in the *World Inspector* and click on ``[Save as Prefab]`` in the context menu. Choose a name in the dialog and confirm by clicking ``[Save]``.

To load a prefab via the editor, head over to the *Prefab Browser*, click and hold the desired prefab and drag it into the viewport. Release the mouse button when you're happy with the spawn position.

Randomized shape and prefab placement
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To quickly place variations of prefabs and voxel shapes with randomized size and rotation parameters, head over to the *Prefab Browser* and click the dice icon. In the *Prefab Randomization* window, set the desired randomization intervals for the size and orientation.

When done, go ahead and place prefabs and shapes as usual in the scene using the *Prefab Browser*. With each spawned shape/prefab, the randomized orientation and size is applied to the resulting root node.

Snapping entities to the ground
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To ease the placement of entities in the scene, it's possible to snap them to the ground. Snapping internally uses a raycast in combination with the extents of the shape.

To snap an entity and its hierarchy to the ground, either click on ``[Edit] => [Snap to Ground]`` or press ``[V]``.

Centering voxel shapes
^^^^^^^^^^^^^^^^^^^^^^

Voxel shapes have their origin the lower left corner of their grid. To center a voxel shape, select it and either press ``[C]`` or click on ``[Edit] => Center Entity``. This operation updates the pivot of the shape to the center of its extents.

Moving an entity to the current cursor position
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Entities can be moved to the current position of the mouse cursor by pressing ``[R]``.

Switching the active camera
^^^^^^^^^^^^^^^^^^^^^^^^^^^

When working with the editor, the `editing_camera` is activated by default. To switch the camera, right-click on the desired camera in the *World Inspector* and select ``[Camera Actions] => [Activate Camera]`` to activate it.

Copying camera transforms
^^^^^^^^^^^^^^^^^^^^^^^^^

Sometimes, it's desirable to copy one camera's transform to another quickly. There are multiple options to achieve this:

- Select the camera from which you want to copy the transform in the *World Inspector*. Then right-click on the camera you want to copy the transform to and select ``[Camera Actions] => [Copy Transform from Selected]``
- Activate the camera you want to copy the transform from and orient and translate it as desired. Then right-click on the camera you want to copy the transform to in the *World Inspector* and select ``[Camera Actions] => [Copy Transfrom from Active]``. This is especially useful if you want to, e.g., copy the current transform of the ``editing_camera`` to the ``game_camera``.

Working with the console
------------------------

The console can be used to show the current log output during runtime and to interact with the settings. Its available while the game is running and in the editor. To open it up, press ``[F2]``. To execute a command, enter it into the console window and press ``[Enter]``.

The following commands are available:

ls
   Lists all settings and their values
load_settings
   Loads and applies all settings from disk (engine and user settings)
save_user_settings
   Writes all settings to the user's settings file. Only settings which differ from their default value are written
<setting_name>
   Prints the current value of the setting with the given name to the console
<setting_name> <value>
   Sets the setting with the given name to the provided value. Settings support values as unsigned integers, floats, boolean values, and strings
   
Importing assets
================

This section briefly the describes the process of importing various asset types.

Importing textures
------------------

IOLITE requires textures to be in the DDS file format. The command line tool ``texconv.exe`` from `DirectXTex <https://github.com/microsoft/DirectXTex>`_ is an excellent option for converting various image formats to DDS. 

.. note:: If you're using Linux, it's possible to run the tool using Wine, using ``wine texconv.exe ...``

All textures are sourced from ``media/images`` in each data source directory and can be accessed during runtime via the filename (**without** the extension).

.. important:: 

   When generating textures for use in IOLITE's UI system, it is important to export them with pre-multiplied alpha enabled. Here's an example of how to correctly convert a UI texture using DirectXTex to the BC7 texture format:

   .. code-block:: console

     texconv.exe -pmalpha -f BC7_UNORM_SRGB my_texture.png

Importing VOX files
-------------------

If you want to add a new voxel asset to, e.g., the default data source, place the file in ``default/media/voxels``. Please make sure to use the correct ``.vox`` file extension. If IOLITE is running while adding new voxel assets, you can trigger a hot-reload by executing ``[Tools] => [Reload Voxel Shapes]`` in the editor. This makes the new assets available without restarting the engine.

Importing scenes from VOX files
-------------------------------

By default, IOLITE scans all the VOX files available in the data sources and makes each found model available as a separate asset. In addition, VOX files also support storing scene information which can then be imported. This is useful for quickly importing scenes that have been authored in external tools like MagicaVoxel or Avoyd.

To import a scene from an available VOX file, open the ``[Import and Export]`` menu, hover ``[Import Scene]``, and finally select the VOX file to import the scene from. The scene hierarchy is added to the world as a new entity named according to the source asset.

Importing palettes
------------------

Palettes can either be imported by adding VOX files to your project or by importing them separately from image files downloaded from sites such as `Lospec <https://lospec.com/>`_ or authored in image editing tools like *Photoshop*. This is especially useful if you want to author your voxel assets directly in IOLITE, e.g., using the :ref:`plugin_voxel_editing` plugin.

.. important:: Each pixel in the palette image is interpreted as a color of the palette. The pixels are read row by row starting from the top left corner until the maximum of 255 palette colors is reached.

To import a palette from an image, select ``[Import and Export] => [Import Palette]`` and choose the desired image file from your file system. Palettes can be inspected and edited via the *Palette Inspector* available via the ``[View]`` menu. See :ref:`palette_inspector` for more details.

Editing resources via Inspectors
================================

.. TODO

IOLITE's exposes some of its resources for editing and inspection via so-called *Inspectors*.

.. _palette_inspector:

Palette Inspector
-----------------

.. image:: /_static/images/palette_inspector.jpg
   
The palette inspector can be used to create, inspect and modify palettes. Besides the palettes you have created manually, either via the editor or code, it also shows all the palettes that have been sourced from your voxel assets.

The general functionality of palettes is described in detail in :ref:`palettes`.

.. _post_effect_inspector:

Post Effect Inspector
---------------------

.. warning:: Coming soon.

.. _particle_inspector:

Particle Inspector
------------------

.. warning:: Coming soon.

IOLITE PRO specific features
============================

This section is dedicated to the additional features IOLITE PRO provides.

Disabling the splash screen
---------------------------

To disable the splash screen, add the following member to your ``app_metadata.json`` file:

.. code-block:: json

  "show_splash_screen": false

Creating voxel assets from meshes (Mesh Voxelizer)
--------------------------------------------------

The mesh voxelizer allows you to create voxel assets from complex 3D meshes.

To voxelize a mesh, export a mesh from your favorite 3D authoring software, like *Blender* or *3ds Max*, in the *glTF* format. After exporting, the meshes, head over to IOLITE's editor and open up the ``[Import and Export]`` menu in the toolbar and hover and click on ``[Voxelize Mesh]``.

Exporting path-traced renders
-----------------------------

To export your path-traced renders, head over to the ``[Render]`` menu in the menu bar and select ``[Export Render]``. With the PRO version, you also have the option to enable *raw color output*, which turns off the display transform, and to select the lossless *EXR* file format which is great for handling post-processing in external tools like *Blender* or *Darktable*.