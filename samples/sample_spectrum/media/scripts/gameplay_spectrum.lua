Sound.load()
World.load()
Node.load()
Random.load()
Math.load()
Entity.load()
VoxelShape.load()

NumRows = 25
Intensity = 2
Delay = 2

function UpdateSpectrum(spectrum, delta_t)
    local s_entity = Entity.find_first_entity_with_name("spectrum")
    local s_shape = VoxelShape.get_component_for_entity(s_entity)

    for x = 1, 50 do
        local amp = spectrum[x]
        local height = amp * 10 * 1.5

        for y = 1, 10 do
            local pos = UVec3((x - 1) * 2 + 1, y - 1, 0)

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
    TimePassed = 0.0
    Cubes = {}

    for z = 1, NumRows do
        Cubes[z] = {}
        for x = 1, NumRows do
            local cube = World.spawn_prefab("spectrum_cube")
            Node.set_world_position(cube, Vec3(x - 1, 0, z - 1))
            Node.update_transforms(cube)

            local mod = 8
            local spawn = ((x - 1) % mod == 0 and (z - 1) % mod ~= 0) or ((z - 1) % mod == 0 and (x - 1) % mod ~= 0)
            if spawn then
                local cube_rigid = World.spawn_prefab("spectrum_rigidbody")
                Node.set_world_position(cube_rigid, Vec3(x - 1, 5, z - 1))
                Node.update_transforms(cube_rigid)
            end

            Cubes[z][x] = {
                node = cube,
                idx = Random.rand_uint_min_max(0, 50) + 1,
                col = 1,
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

    TimePassed = TimePassed + delta_t
    if TimePassed > Delay and not Music then
        Music = Sound.play_sound_effect("sample_spectrum")
    end

    for i = 1, #Cubes do
        local cs = Cubes[i]
        for j = 1, #cs do
            local cube = cs[j]
            local amp = spectrum[cube.idx]
            local pos = Node.get_world_position(cube.node)
            local next_col = 1 + Math.floor(amp * 6)
            local shape = VoxelShape.get_component_for_entity(Node.get_entity(cube.node))

            if cube.has_color and not (next_col == cube.col) then
                cube.col = next_col

                VoxelShape.fill(shape, UVec3(1, 0, 1), UVec3(8, 9, 8), cube.col)
                VoxelShape.voxelize(shape)
            end

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
    if Music then
        Sound.stop_sound_effect(Music)
    end
end

--- Event callback
---@param entity Ref The ref of the entity the script component is attached to.
---@param events table List of all events.
function OnEvent(entity, events)
end
