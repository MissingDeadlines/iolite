.. _plugin_terrain_generator:

⛰️ Terrain Generator
=====================

This section serves as a reference guide for the Terrain Generator Plugin which is a simple helper to generate voxel shapes from heightmap images.

Basic Overview
=====================
The Terrain Plugin allows you to generate a landscape at runtime, using heightmap data. There are currently two different ways of generating a landscape:

- Generate from **Data**
- Generate from **Image**

Generate From Data
=====================
.. code-block:: cpp

    auto generate_from_data(const io_plugin_terrain_heightmap_pixel* heightmap,
                            const io_uint32_t size, const char* palette_name,
                            io_float32_t max_height, io_float32_t voxel_size);

To use the  **`generate_from_data`** function, you need to provide the following inputs:

- **Heightmap Array:** holds heightmap pixel data.
- **Terrain Size (in pixels):** The total size of the terrain.
- **Palette Name:** The name of the colour palette to be used.
- **Maximum Height:** The maximum height allowed.
- **Voxel Size:** The size of each voxel.

Heightmap Pixel
------------------
.. code-block:: cpp

    // A single pixel of a heightmap
    //----------------------------------------------------------------------------//
    typedef struct
    {
      io_uint32_t internal;
    } io_plugin_terrain_heightmap_pixel;
    
    //----------------------------------------------------------------------------//
    inline io_plugin_terrain_heightmap_pixel
    io_plugin_terrain_create_heightmap_pixel(io_float32_t height,
                                             io_float32_t grass_height,
                                             io_uint8_t palette_index)
    {
      const io_uint32_t v_height = io_min(height * 255.0f, 255u);
      const io_uint32_t v_grass_height = io_min(grass_height * 255.0f, 255u);
      const io_uint32_t v_palette_index = palette_index;
    
      io_plugin_terrain_heightmap_pixel p;
      p.internal = v_height | (v_grass_height << 8u) | (palette_index << 16u);
    
      return p;
    }

For the terrain generation to know the height data of each position, we store all the data required into an array of heightmap_pixel's. Each 'heightmap_pixel' holds the necessary data related to the pixel's elevation.

It holds the following data:

- **Height:** The height of the pixel clamped between 0 and 255.
- **Grass Height:** the height level of the grass clamped between 0 and 255.
- **Palette Index:** The colour of which the voxels will be in the column.


Creating Heightmap
------------------
To create our heightmap data, we will need to use the Noise Module and Generate the height level based on the noise values. Here is the official example script used in IOLITE to create a heightmap. 

.. code-block:: lua

  HeightmapGenerator = coroutine.create(function()
    
    -- Table to hold the generated heightmap pixels.
    local heightmap = {}

    -- Scale factor for the noise function.
    local noise_scale = 0.005

    -- Loops over each chunk of the terrain.
    for chunk = 1, NumChunks do
      
      -- For each row within the current chunk.
      for y = 1, Size / NumChunks do
        
        -- For each column of the map width.
        for x = 1, Size do
          
          -- Creates a position vector for the noise function.
          local noise_pos = Vec4((x - 1) * noise_scale,
            (y - 1 + (chunk - 1) * Size / NumChunks) * noise_scale, 0.0, 0.0)

          -- We generate a base height using simplex noise, normalizing to 0 - 1.
          -- We shift the values from -1 - 1 to 0 - 1
          local height = (Noise.simplex(noise_pos) * 0.5 + 0.5) * 0.5

          -- We scales the noise position for additional layered noise.
          -- We Double the noise scale so that the noise is less dense
          noise_pos.x = noise_pos.x * 2.0
          noise_pos.y = noise_pos.y * 2.0

          -- Adds secondary noise for more detail.
          height = height + (Noise.simplex(noise_pos) * 0.5 + 0.5) * 0.25 -- Adds noise to height.

          -- We continue scaling noise and accumulating height from further octaves.
          noise_pos.x = noise_pos.x * 2.0
          noise_pos.y = noise_pos.y * 2.0
          -- Adds more noise
          height = height + (Noise.simplex(noise_pos) * 0.5 + 0.5) * 0.125 -- Adds noise to height.
					-- We continiue to scale the noise
          noise_pos.x = noise_pos.x * 2.0
          noise_pos.y = noise_pos.y * 2.0
          -- Then we add the final layer of noise
          height = height + (Noise.simplex(noise_pos) * 0.5 + 0.5) * 0.125 * 0.5

          -- Adds a base offset and squares the height to emphasize taller areas.
          height = 0.25 + height * height

          -- Picks a palette index for coloring the terrain based on height.
          -- The palette index is based on the height value.
          -- Since the palette is 24 colors, the height is multiplied by 23.
          -- This makes sure that its in the correct range for the palette.
          local palette_idx = 0 + Math.floor(height * 23.0)

          -- Currently no grass height for simplicity.
          local grass_height = 0.0

          -- Inserts the final pixel data (height, grass, palette index) into the heightmap table.
          -- HeightmapPixel is a custom struct defined in the Terrain module.
          -- heightmap pixel struct ensures that the height and grass height are clamped to a max of 255;
          table.insert(heightmap, Terrain.HeightmapPixel(height, grass_height, palette_idx))
        end
      end

      -- Updates the generation progress percentage for the current chunk.
      Progress = (chunk - 1) / NumChunks * 100

      -- Yields back to allow other processes to run before continuing.
      coroutine.yield()
    end

    -- Final yield returns the completed heightmap.
    coroutine.yield(heightmap)
  end)

Generating Terrain
------------------
Now that we have calculated our heightmap we can now generate our terrain. We do this by calling the 'generate_from_data' function:

.. code-block:: lua

    TerrainNode = Terrain.generate_from_data(Heightmap, Size, "terrain", 256.0, 0.1) -- heightmap, size of terrain, palette, max_height, voxel size

Generate from image
===================

.. code-block:: cpp

    auto generate_from_image(const char* heightmap_name, const char* palette_name,
                         io_float32_t max_height, io_float32_t voxel_size)

Generate from image requires the following data:

- **Name of Heightmap Image:** The name of the image file
- **Palette Name:** The name of the colour palette to be used.
- **Maximum Height:** The maximum height allowed.
- **Voxel Size:** The size of each voxel.

When importing your heightmap image its important to place it in the correct path. “/media/heightmaps/” as this is where the function will be searching for the file name.

Generating Terrain
-------------------
Now that we have our heightmap imported we can now call the ‘generate_from_image’ function.

.. code-block:: lua

    TerrainNode = Terrain.generate_from_image("heightmap", "terrain", 256.0, 0.1)


After following the steps in this documentation, you should be able to have a better understanding on how the terrain plugin works and how the example code provided by IOLITE works. These concepts can be expanded on, to create more advanced terrain generations. Such as using the height data to spawn objects.
