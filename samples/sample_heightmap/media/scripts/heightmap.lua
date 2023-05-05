Math.load()
Terrain.load()
Noise.load()
Node.load()
UI.load()

---Called once when the script component becomes active.
---@param entity Ref The ref of the entity the script component is attached to.
function OnActivate(entity)
    local heightmap = {}
    local size = 1024
    local noise_scale = 0.005
    for i = 1, size * size do
        local x = (i - 1) % size
        local y = Math.floor((i - 1) / size)
        local height = Noise.simplex(Vec4(x * noise_scale, y * noise_scale, 0.0, 0.0))
        height = 0.1 + (0.5 + height * 0.5) * 0.9

        local palette_idx = 1
        if height > 0.75 then
            palette_idx = 0
        end
        local grass_height = 0.0

        -- Pixel requires: 
        -- Terrain height [0, 1], grass height [0, 1], and the palette index [0, 255]
        table.insert(heightmap, Terrain.create_pixel(height, grass_height, palette_idx))
    end

    -- Requires: The heightmap as a table, the width/height (has to be square), 
    -- the name of a voxel shape to source the palette from, the maximum 
    -- height in voxels, and the scale (size of a single voxel)
    TerrainNode = Terrain.generate_from_data(heightmap, size, "cube_1m", 256.0, 0.1)
end

--- Called every frame.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Tick(entity, delta_t)
    UI.draw_image("splash", Vec2(0.99, 0.01), Vec2(0.2, -1.0), Vec4(1.0), Vec2(1.0, 0.0))
end

--- Called at the frequency of the update interval specified in the component.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Update(entity, delta_t)
end

--- Called once when the script component becomes inactive.
---@param entity Ref The ref of the entity the script component is attached to.
function OnDeactivate(entity)
    Node.destroy(TerrainNode)
    TerrainNode = nil
end

--- Event callback
---@param entity Ref The ref of the entity the script component is attached to.
---@param events table List of all events.
function OnEvent(entity, events)
end
