.. _palettes:

Palettes
========

Palettes are used to assign material parameters to the values of voxel shapes. As mentioned in the previous sections, shapes utilize 8-bit values offering a total of 256 possible unique values. A value of zero is reserved for marking empty/air voxels. Accordingly, a single palette supports storing up to 255 different materials. Each voxel shape can utilize a unique palette, but it is not possible to use multiple palettes for a single shape.

.. important:: As mentioned previously, due to the fact that a value of zero is reserved for empty/air voxels, the voxel values 1-255 map to the palette indices 0-254.
   
Palettes can be edited either programmatically or via the :ref:`palette_inspector` in the editor, which is shown in the following screenshot:

.. image:: /_static/images/palette_inspector.jpg
   
Voxel assets also contain palettes which are treated the same as the custom palettes you have created. Asset palettes are named according to their source assets. To overwrite the palette of a voxel asset, simply create a custom palette named like the palette you would like to overwrite. Custom palettes are prioritized during loading.

The following sections describe each of the material parameters a single entry in the palette supports.

Rendering material parameters
-----------------------------

Color
   The albedo color of this material.
Roughness
   Defines the roughness/smoothness of the material.
Matalmask
   Defines whether this material is a nonmetal (value of 0) or metal (value of 1). Values in between can be used for defining hybrid materials like, e.g., rusty metals.
Emissive
   The emissive intensity of this material.
   
The following screenshot shows the impact of the roughness parameter for a red/white metal and nonmetal:
   
.. image:: /_static/images/palette_spheres.jpg

Physics material parameters
---------------------------

Hardness
   The hardness of the material which is evaluated in the context of the physics-based destruction features.