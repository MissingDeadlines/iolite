Log.load()
DebugGeometry.load()
Pathfinding.load()
Entity.load()
CharacterController.load()
Random.load()
UI.load()

function UpdatePathfinding(context)
    -- Collect the components of our character
    local char = Entity.find_first_entity_with_name(context.char_name)
    local char_node = Node.get_component_for_entity(char)
    local char_pos = Node.get_world_position(char_node)
    local char_cct = CharacterController.get_component_for_entity(char)

    -- If we've got no random cell yet, select a new one
    if not context.target_cell then
        context.target_cell = Maze.get_random_cell()
        context.target_position = Maze.get_center_position(context.target_cell)

        -- Start calculating a path from the current position of the character
        -- to the chosen random target position
        local settings = Pathfinding.create_settings()
        settings.capsule_radius = 0.4
        context.path = Pathfinding.find_path(char_pos, context.target_position, settings)
    end

    -- Process our path
    if context.path then
        -- Draw the path and a sphere at the target position
        Pathfinding.draw_path(context.path, context.color, true)
        DebugGeometry.draw_sphere(context.target_position, 1.0, context.color, false)

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

--- Called every frame.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Tick(entity, delta_t)
    -- Draw the logo
    UI.draw_image("splash", Vec2(0.99, 0.01), Vec2(0.2, -1.0), Vec4(1.0), Vec2(1.0, 0.0))

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

    UI.draw_text(string.format("Active Agents: %i", num_active_agents), Vec2(0.01, 0.98), Vec2(0.0, 0.0), Vec4(1.0), 0)
end

--- Called once when the script component becomes inactive.
---@param entity Ref The ref of the entity the script component is attached to.
function OnDeactivate(entity)
    -- Clean up and destroy all existing paths
    for i = 1, #PathfindingContexts do
        local ctxt = PathfindingContexts[i]
        if ctxt.path then
            Pathfinding.destroy_path(ctxt.path)
        end
    end
end

--- Event callback
---@param entity Ref The ref of the entity the script component is attached to.
---@param events table List of all events.
function OnEvent(entity, events)
end
