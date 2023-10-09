-- MIT License
--
-- Copyright (c) 2023 Missing Deadlines (Benjamin Wrensch)
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
-- SOFTWARE.

Sound.load()
World.load()
Node.load()
Random.load()
Math.load()
Entity.load()
VoxelShape.load()
Settings.load()

NumRows = 25
Intensity = 2
Delay = 2

function UpdateSpectrum(spectrum, delta_t)
  local s_entity = Entity.find_first_entity_with_name("spectrum")
  local s_shape = VoxelShape.get_component_for_entity(s_entity)

  -- Update the bars of the spectrum visualizer
  for x = 1, 50 do
    local amp = spectrum[x]
    local height = amp * 10 * 1.5

    -- Fill/color voxels along the height according to the amplitude
    for y = 1, 10 do
      local pos = U8Vec3((x - 1) * 2 + 1, y - 1, 0)

      local color = 0
      if y <= height then
        color = Math.floor(1 + y / 10 * 5)
      end

      VoxelShape.set_unsafe(s_shape, pos, color)
    end
  end

  VoxelShape.voxelize(s_shape)
end

---Called once when the script component becomes active.
---@param entity Ref The ref of the entity the script component is attached to.
function OnActivate(entity)
  -- Ensure all shapes are updated as quickly as possible
  NumToVoxelizePerFrameBefore = Settings.get_uint("vs_num_to_voxelize_per_frame")
  Settings.set_uint("vs_num_to_voxelize_per_frame", 1024)

  TimePassed = 0.0
  Cubes = {}

  -- Generate floor cubes and some rigid bodies on top
  for z = 1, NumRows do
    Cubes[z] = {}
    for x = 1, NumRows do
      -- Spawn floor cubes
      local cube = World.spawn_prefab("spectrum_cube")
      Node.set_world_position(cube, Vec3(x - 1, 0, z - 1))
      Node.update_transforms(cube)

      -- Spawn less rigid bodies
      local mod = 8
      local spawn = ((x - 1) % mod == 0 and (z - 1) % mod ~= 0) or ((z - 1) % mod == 0 and (x - 1) % mod ~= 0)
      if spawn then
        local cube_rigid = World.spawn_prefab("spectrum_rigidbody")
        Node.set_world_position(cube_rigid, Vec3(x - 1, 5, z - 1))
        Node.update_transforms(cube_rigid)
      end

      -- Store the floor cubes so we can access them easily later on
      Cubes[z][x] = {
        node = cube,
        -- Assign to a random bin of the spectrum
        idx = Random.rand_uint_min_max(0, 50) + 1,
        col = 1,
        -- Color every other cube
        has_color = Random.rand_float_min_max(0, 1) > 0.5
      }
    end
  end
end

--- Called every frame.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Tick(entity, delta_t)
  local spectrum = Sound.get_spectrum()
  UpdateSpectrum(spectrum, delta_t)

  -- Start music after a short delay
  TimePassed = TimePassed + delta_t
  if TimePassed > Delay and not Music then
    Music = Sound.play_sound_effect("sample_spectrum")
  end

  -- Iterate over our floor cubes
  for i = 1, #Cubes do
    local cs = Cubes[i]
    for j = 1, #cs do
      local cube = cs[j]
      local amp = spectrum[cube.idx]
      local pos = Node.get_world_position(cube.node)
      local next_col = 1 + Math.floor(amp * 6)
      local shape = VoxelShape.get_component_for_entity(Node.get_entity(cube.node))

      -- Adjust the color of the floor cubes according to the amplitude of the spectrum
      if cube.has_color and not (next_col == cube.col) then
        cube.col = next_col

        VoxelShape.fill(shape, U8Vec3(1, 0, 1), U8Vec3(8, 9, 8), cube.col)
        VoxelShape.voxelize(shape)
      end

      -- Modulate y-position of the cubes according to the amplitude
      pos.y = Math.pow(amp, 2.0) * Intensity
      Node.set_world_position(cube.node, pos)
      Node.update_transforms(cube.node)
    end
  end
end

--- Called at the frequency of the update interval specified in the component.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Update(entity, delta_t)
end

--- Called once when the script component becomes inactive.
---@param entity Ref The ref of the entity the script component is attached to.
function OnDeactivate(entity)
  -- Stop playing music
  if Music then
    Sound.stop_sound_effect(Music)
  end

  -- Reset setting
  Settings.set_uint("vs_num_to_voxelize_per_frame", NumToVoxelizePerFrameBefore)
end

--- Event callback
---@param entity Ref The ref of the entity the script component is attached to.
---@param events table List of all events.
function OnEvent(entity, events)
end
