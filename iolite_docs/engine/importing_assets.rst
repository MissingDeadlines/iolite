.. _importing_assets:

Importing assets
================

This section briefly the describes the process of importing various asset types.

Importing textures
------------------

IOLITE requires textures to be in the DDS file format. The command line tool ``texconv.exe`` from `DirectXTex <https://github.com/microsoft/DirectXTex>`_ is an excellent option for converting various image formats to DDS. 

.. note:: If you're using Linux, it's possible to run the tool using Wine, using ``wine texconv.exe ...``

All textures are sourced from ``<data source>/media/images``.

.. important:: 

   When generating textures for use in IOLITE's UI system, it is important to export them with pre-multiplied alpha enabled. Here's an example of how to correctly convert a UI texture using *DirectXTex* to the BC7 texture format:

   .. code-block:: console

     texconv.exe -pmalpha -f BC7_UNORM_SRGB my_texture.png

Importing VOX files
-------------------

If you want to add a new voxel asset to, place the file in ``<data source>/media/voxels``. Please make sure to use the correct ``.vox`` file extension. If IOLITE is running while adding new voxel assets, you can trigger a hot-reload by executing ``[Tools] => [Reload Voxel Shapes]`` in the editor. This makes the new assets available without restarting the engine.

Importing scenes from VOX files
-------------------------------

By default, IOLITE scans all the VOX files available in the data sources and makes each found model available as a separate asset. In addition, VOX files also support storing scene information which can then be imported. This is useful for quickly importing scenes that have been authored in external tools like MagicaVoxel or Avoyd.

To import a scene from an available VOX file, open the ``[Import and Export]`` menu, hover ``[Import Scene]``, and finally select the VOX file to import the scene from. The scene hierarchy is added to the world as a new entity named according to the source asset.

Importing palettes
------------------

Palettes can either be imported by adding VOX files to your project or by importing them separately from image files downloaded from sites such as `Lospec <https://lospec.com/>`_ or authored in image editing tools like *Photoshop*. This is especially useful if you want to author your voxel assets directly in IOLITE, e.g., using the :ref:`plugin_voxel_editing` plugin.

.. important:: Each pixel in the palette image is interpreted as a color of the palette. The pixels are read row by row starting from the top left corner until the maximum of 255 palette colors is reached.

To import a palette from an image, open up the editor, select ``[Import and Export] => [Import Palette]``, and choose the desired image file from your file system. Palettes can be inspected and edited via the *Palette Inspector* available via the ``[View]`` menu.

Importing animations
--------------------

Animations can be imported via glTF binaries. To make an animation available, simply place the file in the ``<data source>/media/animations`` directory and ensure it's suffixed with ``.glb``.  Use ``[Tools] => [Reload Animations]`` to hot-reload changed and freshly added animations during runtime.