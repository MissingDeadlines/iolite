Log.load()
DebugGeometry.load()
Pathfinding.load()
Entity.load()
CharacterController.load()

Maze = Utils.require("maze_generator")

ExecutePathfinding = false

function UpdatePathfinding(context)
  local char = Entity.find_first_entity_with_name(context.char_name)
  local char_node = Node.get_component_for_entity(char)
  local char_pos = Node.get_world_position(char_node)
  local char_cct = CharacterController.get_component_for_entity(char)

  if not context.target_cell then
    context.target_cell = Maze.get_random_cell()
    context.target_position = Maze.get_center_position(context.target_cell)
    context.path = Pathfinding.find_path(char_pos, context.target_position)
  end

  if context.path then
    Pathfinding.draw_debug_geometry(context.path, false, true)
    DebugGeometry.draw_sphere(context.target_position, 1.0, Vec4(1.0, 0.0, 0.0, 1.0), false)

    if not context.next_position then
      local pos, success = Pathfinding.get_next_position_on_path(context.path)

      if success then
        context.next_position = pos
      end
    end

    if context.next_position then
      local to_target = Math.vec_sub(context.next_position, char_pos)
      local dist = Math.vec_length(to_target)

      if dist > 0 then
        to_target = Math.vec_scale(1.0 / dist, to_target)
      end

      local move_vec = Math.vec_scale(8.0, to_target)

      CharacterController.move(char_cct, move_vec)

      if dist < 1.0 then
        local pos, success = Pathfinding.get_next_position_on_path(context.path)

        if success then
          context.next_position = pos
        else
          context.target_cell = nil
          context.next_position = nil

          Pathfinding.destroy_path(context.path)
          context.path = nil
        end
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

  Contexts = {}
  for i=1,6 do
    table.insert(Contexts, {char_name=string.format("char%i", i)})
  end
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
    for i=1,#Contexts do
      UpdatePathfinding(Contexts[i])
    end
  end
end

--- Called once when the script component becomes inactive.
---@param entity Ref The ref of the entity the script component is attached to.
function OnDeactivate(entity)
    for i=1,#Contexts do
      if Contexts[i].path then
        Pathfinding.destroy_path(Contexts[i].path)
      end
    end
end

--- Event callback
---@param entity Ref The ref of the entity the script component is attached to.
---@param events table List of all events.
function OnEvent(entity, events)
end
