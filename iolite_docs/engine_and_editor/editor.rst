Basic Usage of the Editor
=========================

This section describes some everyday tasks when working with IOLITE and its editor.

Running your Game
-----------------

When launching IOLITE in its default configuration, the application starts in the editing game state. All game logic, most importantly scripts and the physics simulation, is only executed when the main game state is active.

.. note:: To make the main game state the default after launch, check the section about the app metadata file for instructions.

To switch to the main game state in the editor, click on ``[Game Mode]`` in the menubar of the editor. When clicking this button, IOLITE stores a snapshot of the current world, switches the active camera from the ``editing_camera`` to the ``game_camera``, and activate the main game state. By pressing ``[F3]`` while the main game state is active, the main game state gets deactivated, the previous state of the world gets restored, and the editor becomes active again.

Working with Entities and Components
------------------------------------

As teased in the introduction of the documentation, entities and components form the basis for creating games with IOLITE. This section covers some basics, like, e.g., creating and removing entities in the editor, how to clone entities, and how entities can be decorated with components. 

Selecting Entities
^^^^^^^^^^^^^^^^^^

Select entities either by clicking on them in the *World Inspector* or visually by clicking on them in the viewport. Visual picking only works if the selection tool is active in the editing toolbar.

Moving and Rotating Entities
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Select the entity you want to transform and select either the transform, rotate, or the combined gizmo from the editing toolbar. Click, hold, and drag the gizmo handles to apply the desired transformation.

Creating and Removing Entities
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To create a new entity in the editor, right-click on the entity in the *World Inspector* you want to attach the new entity to. In the context menu, either select ``[Attach New Entity]`` or ``[Attach New Entity (With Component)]`` to create an entity decorated with the selected component from the start.

Remove entities by selecting ``[Remove Entity]`` from the context menu or pressing ``[Del]`` after selecting it in the *World Inspector*.

Adding and Removing Components
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To add a component to an existing entity, right-click on it in the *World Inspector*, select ``[Add Component]`` in the context menu, and select the component you'd like to add.

To remove a component, select ``[Remove Component]`` in the context menu, and select the name of the component you want to remove.

Cloning Entities
^^^^^^^^^^^^^^^^

Clone entities by selecting them and choosing ``[Clone Entity]`` from the context menu. It's also possible to clone entities by pressing ``[Ctrl + D]``.

.. note:: Cloning entities also copies the whole node hierarchy and all the attached components.

Renaming Entities
^^^^^^^^^^^^^^^^^

Select the entity you want to rename and select ``[Rename Entity]`` from the context menu. Enter the new name in the text field and finalize the operation by clicking ``[Rename]``.

Switching the Active Camera
---------------------------

When working with the editor, the `editing_camera` is activated by default. To switch the camera, right-click on the desired camera in the *World Inspector* and select ``[Activate Camera]`` to activate it.

Placing Voxel Shapes in the World
---------------------------------

To quickly position a voxel shape in the world, head over to the *Prefab Browser*. Click and hold the desired shape and drag it into the viewport. Release the mouse button when you're happy with the position.

Another option is to manually create an entity with an attached *Voxel Shape* component and pick the desired shape via the component's properties.

Saving and Loading Prefabs
--------------------------

Prefabs are hierarchies of entities that can be saved to and loaded from disk. They can either be used to ease editing or to create modules, like, e.g., a character that gets spawned using a script.

To create a prefab, select the root entity in the *World Inspector* and click on ``[Save as Prefab]`` in the context menu. Choose a name in the dialog and confirm by clicking ``[Save]``.

To load a prefab via the editor, head over to the *Prefab Browser*, click and hold the desired prefab and drag it into the viewport. Release the mouse button when you're happy with the spawn position.

Randomized Shape and Prefab Placement
-------------------------------------

To quickly place variations of prefabs and voxel shapes with randomized size and rotation parameters, head over to the *Prefab Browser* and click the dice icon. In the prefab randomization window, set the desired randomization intervals for the size and orientation.

When done, go ahead and place prefabs and shapes as usual in the scene. With each spawned shape/prefab, the randomized orientation and size is applied to the resulting root node.

Snapping Entities to the Ground
-------------------------------

To ease the placement of entities in the scene, it's possible to snap them to the ground using the underlying physics geometry. The snapping uses a ray-cast and the bounds of the object.

To snap an entity to the ground, either click on ``[Edit] => [Snap Entity to Ground]`` or press ``[V]``. In addition, it's also possible to snap all entities of a hierarchy independently to the ground, using ``[Edit] => [Snap Hierarchy to Ground]``.

Working with the Console
------------------------

The console can be used to show the current log output during runtime and to interact with the settings. Its available while the game is running and in the editor. To open it up, press ``[F2]``. To execute a command, enter it into the console window and press ``[Enter]``.

The following commands are available:

``ls``
   Lists all settings and their values
``load_settings``
   Loads and applies all settings from disk (engine and user settings)
``save_user_settings``
   Writes all settings to the user's settings file. Only settings which differ from their default value are written
``[setting_name]``
   Prints the current value of the setting to the console
``[setting_name] [value]``
   Sets the given setting to the provided value. Settings support values as unsigned integers, floats, boolean values, and strings

Importing Textures
------------------

IOLITE requires textures to be in the DDS file format. The command line tool ``texconv.exe`` from `DirectXTex <https://github.com/microsoft/DirectXTex>`_ is an excellent option for converting various image formats to DDS. 

.. note:: If you're using Linux, it's possible to run the tool using Wine, using ``wine texconv.exe ...``

All textures are sourced from ``media/images`` in each data source directory.

The following extra requirements apply when converting textures for usage with the UI system:

* UI textures need to be stored with pre-multiplied alpha
* UI textures must not contain any additional MIP levels (besides the first one, of course)

Here's an example of how to correctly convert a UI texture using DirectXTex:

.. code-block:: console

  texconv.exe -y -m 1 -pmalpha -f BC7_UNORM_SRGB my_texture.png

Importing VOX Files
-------------------

If you want to add a new voxel asset to, e.g., the default data source, place the file in ``default/media/voxels``. Please make sure to use the correct ``.vox`` file extension.

If IOLITE is running while adding new voxel assets, you can trigger a hot-reload by executing ``[Tools] => [Reload Voxel Shapes]`` in the editor.

Importing Scenes from VOX Files
-------------------------------

To import a scene from a VOX file, open the ``[File]`` menu and select ``[Import Scene from VOX]``. Select the VOX file to import the scene from.

.. note:: Note that only scenes from VOX files can be loaded which are already present in the ``media/voxels`` directory; see the previous section.

The imported scene is attached to a new entity named according to the voxel asset it has been imported from.

Components
==========

This section serves as a glossary for all the available core components.

.. note:: The properties of the components are directly documented in the editor and can be accessed by hovering over the property name in the entity inspector.

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

Joint
-----

Joints can be used to connect voxel shapes with varying constraints. The component offers various types of joints to create various physics-based effects.

Kill Plane
----------

Kill planes can be used to automatically destroy voxel shapes which drop below the y-position of the node the component is attached to.

To ensure that the whole shape is below the plane before it is destroyed, the shape's bounding sphere is used. The logic is implemented as follows:

.. code-block:: lua

  if shape_bounds_center.y + shape_bounds_radius < kill_plane_position.y then
    -- Despawn voxel shape
  end

Light
-----

Use this component to add lights to the scene.

Mesh 
----

Mesh components are used to render triangle meshes with different materials. They are mainly used to render the sky sphere and water planes with the according materials.

Additional meshes can be provided by adding OBJ files to the ``media/meshes`` directory in any data source. Currently, only materials named ``default``, ``water``, or ``sky`` are supported.

Node
----

Node components are attached to every entity by default and provide a 3D-transformation, e.g., a position, orientation, and size. They also make it possible to create hierarchies of entities.

Post Effect Volume
------------------

Post effect volumes are used to add post effects to certain areas within the world. They are applied hierarchically based on their priority.

Sound 
-----

Use this component to play sound effects at certain positions in the world.

Script
------

This component can be used to add Lua scripts to the world.

Tag
---

Can be used to add tags to entities. Entities can then be looked up based on the tags via the scripting interface.

Text
----

Use this component to render text at different positions in the world. Also serves as a great tools for adding notes in the world in the editor.

Voxel Shape
-----------

Voxel shapes are in charge of rendering and simulating everything voxel-related. Shapes are either initialized from authored VOX files available in the data sources, or they are procedurally filled with content like, e.g., terrain using a script.

In addition, voxel shapes support the generation of support structures and fracturing for physics-based destruction effects.

Flipbook Animation
------------------

Use this component to animate voxel shapes using flipbook-style animations. Flipbook animations operate by changing the voxel data of the corresponding shape component on a frame by frame basis.

IOLITE PRO Specific Features
============================

This section is dedicated to the additional features IOLITE PRO provides.

Disabling the Splash Screen
---------------------------

To disable the splash screen, add the following member to your ``app_metadata.json`` file:

.. code-block:: json

  "show_splash_screen": false

Creating Voxel Assets from Meshes (Mesh Voxelizer)
--------------------------------------------------

The mesh voxelizer allows you to create voxel assets from complex 3D meshes.

To voxelize a mesh, export a mesh from your favorite 3D authoring software, like *Blender* or *3ds Max*, in the *GlTF* format. After exporting, the meshes, head over to IOLITE's editor and open up the ``[Import and Export]`` menu in the toolbar and hover and click on ``[Voxelize Mesh]``.

Exporting Path-Traced Renders
-----------------------------

To export your path-traced renders, head over to the ``[Render]`` menu in the menu bar and select one of the following options:

``[Export Render]``
   Exports the path-traced render without additional processing in the EXR file format.
``[Export and Denoise Render]``
   Exports the path-tracer render, denoises the result using Intel's Open Image Denoise (OIDN) and stores it using the EXR file format.