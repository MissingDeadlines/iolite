World.load()
Node.load()
Math.load()
Log.load()
Random.load()

RoomSize = 2.9
Extent = 30

function SpawnWall(pos, dir)
  local wall_n = World.spawn_prefab(string.format("wall_%s", dir)) 
  Node.set_world_position(wall_n, Math.vec_add(pos, Vec3(0.0, 0.4, 0.0)))
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

    Cells[cell_idx_x + 1] = {}

    local idx_y = 0
    for y=-Extent/2, Extent/2 - 1 do
      local cell_idx_y = Extent - 1 - idx_y

      local n = SpawnWall(Vec3(x*RoomSize, 0.0, y*RoomSize), "n")
      local e = SpawnWall(Vec3(x*RoomSize, 0.0, y*RoomSize), "e")
      local s = SpawnWall(Vec3(x*RoomSize, 0.0, y*RoomSize), "s")
      local w = SpawnWall(Vec3(x*RoomSize, 0.0, y*RoomSize), "w")

      Cells[cell_idx_x + 1][cell_idx_y + 1] = { n=n, e=e, s=s, w=w }
      idx_y = idx_y + 1
    end

    idx_x = idx_x + 1
  end
end

Maze = {}

function Maze.isCellValid(cell)
  return cell.x >= 1 and cell.y >= 1 and cell.x <= Extent and cell.y <= Extent
end

function Maze.addToFrontier(cell)
  if not Maze.isCellValid(cell) then 
    return
  end

  local gc = Maze.grid[cell.x][cell.y]
  if not gc.frontier and not gc.inside then
    gc.frontier = true
    table.insert(Maze.frontier, cell)
  end
end

function Maze.neighbors(cell)
  local neighbors = {}
  local coords = {
    {x=cell.x + 1, y=cell.y},
    {x=cell.x - 1, y=cell.y},
    {x=cell.x, y=cell.y + 1},
    {x=cell.x, y=cell.y - 1},
  }

  for i=1, #coords do
    if Maze.isCellValid(coords[i]) and Maze.grid[coords[i].x][coords[i].y].inside then
      table.insert(neighbors, coords[i])
    end
  end

  return neighbors
end

function Maze.mark(cell)
  Maze.grid[cell.x][cell.y].inside = true
  Maze.addToFrontier({x=cell.x + 1, y=cell.y})
  Maze.addToFrontier({x=cell.x - 1, y=cell.y})
  Maze.addToFrontier({x=cell.x, y=cell.y + 1})
  Maze.addToFrontier({x=cell.x, y=cell.y - 1})
end

function Maze.advance()
  if #Maze.frontier > 0 then
    local idx = Random.rand_uint_min_max(1, #Maze.frontier + 1)
    cell = table.remove(Maze.frontier, idx)
    neighbors = Maze.neighbors(cell)
    if #neighbors > 0 then
      local n_idx = Random.rand_uint_min_max(1, #neighbors + 1)
      n = neighbors[n_idx]

      ConnectCells(n, cell)
      Maze.mark(cell)
    end
  end
end

---Called once when the script component becomes active.
---@param entity Ref The ref of the entity the script component is attached to.
function OnActivate(entity)
  GenerateWorld()

  Maze.frontier = {}
  Maze.grid = {}
  for x=1, Extent do
    Maze.grid[x] = {}
    for y=1, Extent do
      Maze.grid[x][y] = {inside=false, frontier=false}
    end
  end

  Maze.mark({x=1, y=1})
end

--- Called every frame.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Tick(entity, delta_t)
  local steps = 2
  for i=1,steps do
    Maze.advance()
  end
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
