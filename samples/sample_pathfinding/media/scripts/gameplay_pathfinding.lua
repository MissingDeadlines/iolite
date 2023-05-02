Log.load()
DebugGeometry.load()
Pathfinding.load()
Entity.load()
CharacterController.load()

Maze = Utils.require("maze_generator")

ExecutePathfinding = false

function UpdatePathfinding()
  local char = Entity.find_first_entity_with_name("char1")
  local char_node = Node.get_component_for_entity(char)
  local char_cct = CharacterController.get_component_for_entity(char)
  local char_pos = Node.get_world_position(char_node)

  if not TargetCell then
    TargetCell = Maze.get_random_cell()
    TargetPosition = Maze.get_center_position(TargetCell)
    Path = Pathfinding.find_path(char_pos, TargetPosition)
  end

  if Path then
    Pathfinding.draw_debug_geometry(Path, false, true)
    DebugGeometry.draw_sphere(TargetPosition, 1.0, Vec4(1.0, 0.0, 0.0, 1.0), false)

    if not NextPosition then
      local pos, success = Pathfinding.get_next_position_on_path(Path)

      if success then
        NextPosition = pos
      end
    end

    if NextPosition then
      local to_target = Math.vec_sub(NextPosition, char_pos)
      local dist = Math.vec_length(to_target)

      if dist > 0 then
        to_target = Math.vec_scale(1.0 / dist, to_target)
      end

      CharacterController.move(char_cct, Math.vec_scale(0.01, to_target))

      if dist < 1.0 then
        NextPosition, _ = Pathfinding.get_next_position_on_path(Path)
      end
    end
  end
end

---Called once when the script component becomes active.
---@param entity Ref The ref of the entity the script component is attached to.
function OnActivate(entity)
  Maze.generate_world()
  Maze.initialize()
  Maze.mark({x=1, y=1})
end

--- Called every frame.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Tick(entity, delta_t)
  -- Increase this to speed up the maze generation
  local steps = 2
  for i=1,steps do
    Maze.advance()
  end

  -- Start executing pathfinding when we're done
  if #Maze.frontier == 0 then
    ExecutePathfinding = true
  end

  if ExecutePathfinding then
    UpdatePathfinding()
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
