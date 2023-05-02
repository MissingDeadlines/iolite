World.load()
Node.load()
Math.load()
Log.load()

RoomSize = 2.9
Extent = 20

function SpawnWall(pos, dir)
  local wall_n = World.spawn_prefab(string.format("wall_%s", dir)) 
  Node.set_world_position(wall_n, pos)
  Node.update_transforms(wall_n)

  return wall_n
end

function RemoveWall(cell, dir)
  Node.destroy(cell[dir])
end

function VecToDir(vec)
  if vec.x == 0 and vec.y == 1 then
    return "n"
  elseif vec.x == 1 and vec.y == 0 then
    return "e"
  elseif vec.x == 0 and vec.y == -1 then
    return "s"
  end

  return "w"
end

function ConnectCells(from, to)
  from_cell = Cells[from.y][from.x] 
  to_cell = Cells[to.y][to.x] 

  vec = {x=to.x-from.x,y=to.y-from.y}
  vec_inv = {x=-vec.x, y=-vec.y}
  dir = VecToDir(vec)
  dir_inv = VecToDir(vec_inv)

  RemoveWall(from_cell, dir)
  RemoveWall(to_cell, dir_inv)
end

function GenerateWorld()
  Cells = {}

  local idx_x = 0
  for x=-Extent/2, Extent/2 - 1 do
    local cell_idx_x = Extent - 1 - idx_x

    Cells[cell_idx_x] = {}

    local idx_y = 0
    for y=-Extent/2, Extent/2 - 1 do
      local cell_idx_y = Extent - 1 - idx_y

      local n = SpawnWall(Vec3(x*RoomSize, 0.0, y*RoomSize), "n")
      local e = SpawnWall(Vec3(x*RoomSize, 0.0, y*RoomSize), "e")
      local s = SpawnWall(Vec3(x*RoomSize, 0.0, y*RoomSize), "s")
      local w = SpawnWall(Vec3(x*RoomSize, 0.0, y*RoomSize), "w")

      Cells[cell_idx_x][cell_idx_y] = { n=n, e=e, s=s, w=w }
      idx_y = idx_y + 1
    end

    idx_x = idx_x + 1
  end
end

---Called once when the script component becomes active.
---@param entity Ref The ref of the entity the script component is attached to.
function OnActivate(entity)
  GenerateWorld()
  ConnectCells({x=0, y=0}, {x=0, y=1})
  ConnectCells({x=0, y=0}, {x=1, y=0})
end

--- Called every frame.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Tick(entity, delta_t)
end

--- Called once when the script component becomes inactive.
---@param entity Ref The ref of the entity the script component is attached to.
function OnDeactivate(entity)
end

--- Event callback
---@param entity Ref The ref of the entity the script component is attached to.
---@param events table List of all events.
function OnEvent(entity, events)
end
