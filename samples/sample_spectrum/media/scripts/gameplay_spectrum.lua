Sound.load()
World.load()
Node.load()
Random.load()
Math.load()

NumRows = 25
Intensity = 2
Delay = 2

---Called once when the script component becomes active.
---@param entity Ref The ref of the entity the script component is attached to.
function OnActivate(entity)
    TimePassed = 0.0
    Cubes = {}

    for z = 1, NumRows do
        Cubes[z] = {}
        for x = 1, NumRows do
            local cube = World.spawn_prefab("cube")
            Node.set_world_position(cube, Vec3(x - 1, 0, z - 1))
            Node.update_transforms(cube)

            local mod = 8
            local spawn = ((x - 1) % mod == 0 and (z - 1) % mod ~= 0) or ((z - 1) % mod == 0 and (x - 1) % mod ~= 0)
            if spawn then
                local cube_rigid = World.spawn_prefab("cube_rigidbody")
                Node.set_world_position(cube_rigid, Vec3(x - 1, 5, z - 1))
                Node.update_transforms(cube_rigid)
            end

            Cubes[z][x] = {cube, Random.rand_uint_min_max(0, 50) + 1}
        end
    end
end

--- Called every frame.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Tick(entity, delta_t)
    local spectrum = Sound.get_spectrum()

    TimePassed = TimePassed + delta_t
    if TimePassed > Delay and not Music then
        Music = Sound.play_sound_effect("sample_spectrum")
    end

    for i = 1, #Cubes do
        local cs = Cubes[i]
        for j = 1, #cs do
            local pos = Node.get_world_position(cs[j][1])
            pos.y = Math.pow(spectrum[cs[j][2]], 2.0) * Intensity
            Node.set_world_position(cs[j][1], pos)
            Node.update_transforms(cs[j][1])
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
