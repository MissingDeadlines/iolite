# Python Scripts

Collection of helper scripts for IOLITE written in Python.

## heightmap_generator.py

```
usage: IOLITE Heightmap Generator [-h] [-o OUTPUT] [--height_pattern HEIGHT_PATTERN] [--splat_pattern SPLAT_PATTERN] [--grass_pattern GRASS_PATTERN] [--max_grass_height MAX_GRASS_HEIGHT]
                                  [--num_sub_materials NUM_SUB_MATERIALS] [--grass_material_mask [GRASS_MATERIAL_MASK ...]] [--max_grass_density MAX_GRASS_DENSITY]
                                  dir_path

Generates a combined heightmap from multiple height, splat and grass maps usable for generating terrain in IOLITE.

positional arguments:
  dir_path              The directory containing the maps to combine.

options:
  -h, --help            show this help message and exit
  -o OUTPUT, --output OUTPUT
                        The path of the combined heightmap to genrate.
  --height_pattern HEIGHT_PATTERN
                        The file pattern for heightmaps, defaults to 'height_*'
  --splat_pattern SPLAT_PATTERN
                        The file pattern for splat maps, defaults to 'splat_*'.
  --grass_pattern GRASS_PATTERN
                        The pattern for grass maps, defaults to 'grass_*'.
  --max_grass_height MAX_GRASS_HEIGHT
                        The maximum height of grass (in voxels). Defaults to '8'.
  --num_sub_materials NUM_SUB_MATERIALS
                        The number of sub materials. A sub material number of, e.g., 8 picks a random material from the first 8 materials in the voxel palette for the first splat map. Defaults to '8'.
  --grass_material_mask [GRASS_MATERIAL_MASK ...]
                        A list of material indexes (e.g. index 0 matches the first splat mask) which should be used to mask out grass.
  --max_grass_density MAX_GRASS_DENSITY
                        The maximum density of grass. A value of 0.5 limits grass to 50% of the area. Defaults to 0.5.

