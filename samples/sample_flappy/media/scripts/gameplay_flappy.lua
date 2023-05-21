UI.load()
Input.load()
Entity.load()
VoxelShape.load()
Log.load()
Math.load()
Node.load()
World.load()
ParticleSystem.load()
Sound.load()
Random.load()
CustomData.load()
SaveData.load()

-- Config
NumPipes = 3
PipeDistance = 3.0
PipeSpacing = 1.5
PipeMaxRandomOffset = 1.5

function GetBird()
    local bird = Entity.find_first_entity_with_name("bird")
    local bird_shape = VoxelShape.get_component_for_entity(bird)
    local bird_node = Node.get_component_for_entity(bird)

    return bird, bird_shape, bird_node
end

function GetGamera()
    local cam = Entity.find_first_entity_with_name("game_camera")
    local cam_node = Node.get_component_for_entity(cam)

    return cam_node
end

-- Returns the cached random offset for the pipe at the given index
function GetRandomOffsetForPipe(idx)
    if not RandomOffsets[idx] then
        RandomOffsets[idx] = Random.rand_float_min_max(0.0, PipeMaxRandomOffset)
    end
    return RandomOffsets[idx]
end

-- Positions the pipe shapes around the bird
function PositionPipes()
    for i = 1, NumPipes do
        local pipe_idx = Score + i
        local pipe = Pipes[i]

        local offset = GetRandomOffsetForPipe(pipe_idx)

        Node.set_world_position(pipe.top, Vec3(-pipe_idx * PipeDistance, 1.7 + PipeSpacing + offset, 7.0))
        Node.set_world_position(pipe.bottom, Vec3(-pipe_idx * PipeDistance, -3.4 + offset, 7.0))
        Node.update_transforms(pipe.top)
        Node.update_transforms(pipe.bottom)
    end
end

function GetNode(name)
    local entity = Entity.find_first_entity_with_name(name)
    return Node.get_component_for_entity(entity)
end

function KillBird(pos)
    -- Apply radius damage, spawn particles, play sound, and end game
    ParticleSystem.spawn_particle_emitter("flappy_explosion", pos, 0.1, true)
    World.radius_damage(pos, 1.0, true)
    State = "End"
    TimePassed = 0.0
    Sound.play_sound_effect("flappy_die")
    Sound.play_sound_effect("flappy_hit")
end

-- Returns true when the "flap key" has been pressed
function GetFlap()
    local flap = Input.get_key_state(Key.kSpace, 0) == KeyState.kClicked
    flap = flap or Input.get_key_state(Key.kControllerButtonA, 0) == KeyState.kClicked
    flap = flap or Input.get_key_state(Key.kMouseLeft, 0) == KeyState.kClicked

    if flap then
        Sound.play_sound_effect("flappy_wing")
    end

    return flap
end

function UpdateCamera()
    -- Let the camera follow the bird
    local _, _, bird_node = GetBird()
    local cam_node = GetGamera()
    local cam_pos = Node.get_world_position(cam_node)
    local bird_pos = Node.get_world_position(bird_node)

    cam_pos.x = bird_pos.x
    Node.set_world_position(cam_node, cam_pos)
    Node.update_transforms(cam_node)
end

function UpdateStateGame()
    UI.draw_text(string.format("%03i", Score), Vec2(0.5, 0.75), Vec2(0.0, 0.0), Vec4(1.0), 1)

    -- Push the bird upwards on flap
    local _, bird_shape, bird_node = GetBird()
    if GetFlap() then
        VoxelShape.apply_force(bird_shape, Vec3(0.0, 200.0, 0.0))
    end

    local bird_pos = Node.get_world_position(bird_node)
    if bird_pos.y < -1.4 or bird_pos.y > 2.5 then
        KillBird(bird_pos)
    end

    local _, _, bird_node = GetBird()
    local bird_pos = Node.get_world_position(bird_node)

    -- Update score
    Score = Math.floor(-bird_pos.x / PipeDistance)
    if PrevScore and PrevScore < Score then
        Sound.play_sound_effect("flappy_point")
    end
    PrevScore = Score
end

function UpdateStateStart()
    UI.draw_text("FLAP TO START!", Vec2(0.5, 0.75), Vec2(0.0, 0.0), Vec4(1.0), 1)

    -- Keep the bird in position
    local _, bird_shape, bird_node = GetBird()
    Node.set_world_position(bird_node, Vec3(0.0, 0.0, 7.0))
    Node.update_transforms(bird_node)
    -- Also counter gravity
    VoxelShape.set_linear_velocity(bird_shape, Vec3(0.0))

    if GetFlap() then
        -- Get the bird moving. Without damping, it will continue
        -- moving in this direction indefinitely
        local _, bird_shape = GetBird()
        VoxelShape.apply_force(bird_shape, Vec3(-100.0, 0.0, 0.0))

        State = "Game"
    end
end

function UpdateStateEnd(delta_t)
    local high_score = Variant.get_uint(CustomData.get(HighScore, 0))

    UI.draw_text("GAME OVER!", Vec2(0.5, 0.5), Vec2(0.0, 0.0), Vec4(1.0), 1)
    UI.draw_text(string.format("SCORE: %03i | HIGH SCORE: %03i", Score, high_score), Vec2(0.5, 0.6), Vec2(0.0, 0.0),
        Vec4(1.0), 1)

    -- Restart the game after some time
    TimePassed = TimePassed + delta_t
    if TimePassed > 3.0 then
        Log.log_info(string.format("Score: %i | High Score: %i", Score, high_score))

        -- Save high score
        if Score > high_score then
            CustomData.set(HighScore, 0, Variant.from_uint(Score))
            SaveData.save_to_user_data("high_score", HighScoreNode)

            Log.log_info("High score updated!")
        end
        World.load_world("sample_flappy")
    end
end

---Called once when the script component becomes active.
---@param entity Ref The ref of the entity the script component is attached to.
function OnActivate(entity)
    -- Default high score
    local high_score_entity = Entity.find_first_entity_with_name("high_score")
    HighScore = CustomData.get_component_for_entity(high_score_entity)
    HighScoreNode = Node.get_component_for_entity(high_score_entity)
    -- Try to load high score
    local saved_high_score_node = SaveData.load_from_user_data("high_score")
    if Ref.is_valid(saved_high_score_node) then
        high_score_entity = Node.get_entity(saved_high_score_node)
        HighScoreNode = saved_high_score_node
        HighScore = CustomData.get_component_for_entity(high_score_entity)

        Log.log_info("Loaded high score from user data...")
    else
        Log.log_info("No saved high score available!")
    end

    State = "Start"
    Score = 0
    RandomOffsets = {}

    Pipes = {}

    -- Collect the pipe shapes
    for i = 1, NumPipes do
        table.insert(Pipes, {
            top = GetNode(string.format("pipe_top%i", i)),
            bottom = GetNode(string.format("pipe_bottom%i", i))
        })
    end
end

--- Called every frame.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Tick(entity, delta_t)
    UI.push_font_scale(1.5)

    -- Draw splash image
    UI.draw_image("splash", Vec2(0.99, 0.01), Vec2(0.2, -1.0), Vec4(1.0), Vec2(1.0, 0.0))

    PositionPipes()

    if State == "Start" then
        UpdateStateStart()
    end
    if State == "Game" then
        UpdateStateGame()
    end
    if State == "End" then
        UpdateStateEnd(delta_t)
    end

    UpdateCamera()

    UI.pop_font_scale()
end

--- Called at the frequency of the update interval specified in the component.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Update(entity, delta_t)
end

--- Called once when the script component becomes inactive.
---@param entity Ref The ref of the entity the script component is attached to.
function OnDeactivate(entity)
end

--- Event callback
---@param entity Ref The ref of the entity the script component is attached to.
---@param events table List of all events.
function OnEvent(entity, events)
    if State == "Game" then
        for i = 1, #events do
            local e = events[i]
            if e.type == "Contact" then
                KillBird(e.data.pos)
                break
            end
        end
    end
end
