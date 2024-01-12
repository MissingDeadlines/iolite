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