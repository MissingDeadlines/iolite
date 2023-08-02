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

Log.load()
DebugGeometry.load()
Pathfinding.load()
Entity.load()
CharacterController.load()
Random.load()
UI.load()
Input.load()
World.load()

function UpdatePathfinding(context)
  -- Collect the components of our character
  local char = Entity.find_first_entity_with_name(context.char_name)
  local char_node = Node.get_component_for_entity(char)
  local char_pos = Node.get_world_position(char_node)
  local char_cct = CharacterController.get_component_for_entity(char)

  -- If we've got no random cell yet, select a new one
  if not context.target_position then
    local target_cell = Maze.get_random_cell()
    context.target_position = Maze.get_center_position(target_cell)
    context.start_position = char_pos

    -- Start calculating a path from the current position of the character
    -- to the chosen random target position
    local settings = Pathfinding.PathSettings()
    settings.capsule_radius = 0.4
    context.path = Pathfinding.find_path(context.start_position, context.target_position, settings)
  end

  -- Process our path
  if context.path then
    -- Draw the path and a sphere at the target position
    Pathfinding.draw_path(context.path, context.color, false)
    DebugGeometry.draw_sphere(context.target_position, 1.0, context.color, false)
    DebugGeometry.draw_sphere(context.start_position, 1.0, context.color, false)

    -- If we've got no next target position yet, try to retrieve a new one
    -- from our path
    if not context.next_position then
      -- Paths are updated asynchronously. If the path is not ready yet,
      -- this function won't succeed
      local pos, success = Pathfinding.get_next_position_on_path(context.path)
      if success then
        -- The path is ready and we can start processing the computed
        -- path
        context.next_position = pos
      end
    end

    -- Time to move towards the next position of the path
    if context.next_position then
      -- Calculate a vector pointing from the current position to the
      -- next position on the path
      local to_target = Math.vec_sub(context.next_position, char_pos)
      -- Cancel out movement along y-axis
      to_target.y = 0
      -- Calculate the distance to the next target
      local dist = Math.vec_length(to_target)
      -- Normalize the vector
      if dist > 0 then
        to_target = Math.vec_scale(1.0 / dist, to_target)
      end

      -- Compute move vector and move our character into the
      -- direction of the position on our path
      local speed = 8.0
      local move_vec = Math.vec_scale(speed, to_target)
      ---@cast move_vec Vec3
      CharacterController.move(char_cct, move_vec)

      -- If we're close to the next position, try to get
      -- the next position on the path
      local dist_threshold = 1.0
      if dist < dist_threshold then
        local pos, success = Pathfinding.get_next_position_on_path(context.path)
        if success then
          -- Move on to the next position on the path
          context.next_position = pos
        else
          -- We've reached our target. Reset everything
          -- so the character choses a new random position
          -- to target
          context.next_position = nil
          context.target_position = nil

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
  Maze = Utils.require("maze_generator")

  -- Generate a random maze
  Maze.generate_world()
  Maze.initialize()
  Maze.mark({
    x = 1,
    y = 1
  })

  -- Contexts store the data needed to compute and update the pathfinding
  PathfindingContexts = {}
  ExecutePathfinding = false
end

function DrawHUD(text)
  UI.push_style_var_float(2, 2.0);
  UI.push_transform(UIAnchor(1.0, -25.0), UIAnchor(1.0, -25.0), UIAnchor(1.0, -25.0), UIAnchor(1.0, -25.0), 0.0);
  UI.draw_text(text, 2, 2, 0);
  UI.pop_transform();
  UI.pop_style_var();
end

--- Called every frame.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Tick(entity, delta_t)
  if Input.get_key_state(Key.kR, 0) == KeyState.kClicked then
    World.load_world("sample_pathfinding")
  end

  -- Generate the maze incrementally. Increase the number of steps
  -- to speed up the process
  local steps = 2
  for i = 1, steps do
    Maze.advance()
  end

  -- Start to execute the pathfinding when the maze is ready
  if Maze.is_ready() and not ExecutePathfinding then
    -- Initialize contexts for eight characters
    local num_chars = 9
    for i = 1, num_chars do
      local name = string.format("char%i", i)
      local char = Entity.find_first_entity_with_name(name)
      local char_node = Node.get_component_for_entity(char)

      -- Calculate and assign random position for this character
      local rand_cell = Maze.get_random_cell()
      local rand_pos = Maze.get_center_position(rand_cell)
      Node.set_world_position(char_node, rand_pos)
      Node.update_transforms(char_node)

      -- Finally initialize and store the context
      local color = Vec4(Random.rand_float_min_max(0.0, 1.0), Random.rand_float_min_max(0.0, 1.0),
        Random.rand_float_min_max(0.0, 1.0), 0.25)
      table.insert(PathfindingContexts, {
        char_name = name,
        color = color
      })
    end

    ExecutePathfinding = true
  end

  local num_active_agents = 0
  if ExecutePathfinding then
    -- Update pathfinding for all contexts
    for i = 1, #PathfindingContexts do
      local ctxt = PathfindingContexts[i]
      UpdatePathfinding(ctxt)
      if ctxt.next_position then
        num_active_agents = num_active_agents + 1
      end
    end
  end

  DrawHUD(string.format("Active Agents: %i", num_active_agents))
end

--- Called once when the script component becomes inactive.
---@param entity Ref The ref of the entity the script component is attached to.
function OnDeactivate(entity)
  -- Clean up and destroy all existing paths
  for i = 1, #PathfindingContexts do
    local ctxt = PathfindingContexts[i]
    if ctxt.path then
      Pathfinding.destroy_path(ctxt.path)
      ctxt.path = nil
    end
  end
end

--- Event callback
---@param entity Ref The ref of the entity the script component is attached to.
---@param events table List of all events.
function OnEvent(entity, events)
end
