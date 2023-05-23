-- Load interfaces
Input.load()
Physics.load()
World.load()
Math.load()
Log.load()
CameraController.load()
Entity.load()
Node.load()
CharacterController.load()
DebugGeometry.load()
VoxelShape.load()
ParticleSystem.load()
UI.load()
Sound.load()

-- Spawns a prefab at the provided distance and applies force/torque to it
function SpawnPrefab(name, distance, force, torque)
    local cam = Entity.find_first_entity_with_name("game_camera")
    local cam_node = Node.get_component_for_entity(cam)

    local origin = Node.get_world_position(cam_node)
    local dir = Math.quat_rotate(Node.get_world_orientation(cam_node), Vec3(0.0, 0.0, 1.0))

    -- Spawn 1m in front of the camera
    local spawn_pos = Math.vec_add(origin, Math.vec_scale(distance, dir))

    local node = World.spawn_prefab(name)
    local shape = VoxelShape.get_component_for_entity(Node.get_entity(node))

    Node.set_world_position(node, spawn_pos)
    Node.update_transforms(node)

    VoxelShape.apply_force(shape, Math.vec_scale(force, dir))
    VoxelShape.apply_torque(shape, Vec3(torque, torque, 0.0))
end

-- Helper function for grabbing voxel shapes in the scene
function GrabVoxelShape(delta_t)
    -- Calculate camera ray
    local cam = Entity.find_first_entity_with_name("game_camera")
    local cam_node = Node.get_component_for_entity(cam)

    local origin = Node.get_world_position(cam_node)
    local dir = Math.quat_rotate(Node.get_world_orientation(cam_node), Vec3(0.0, 0.0, 1.0))

    -- Search for a new shape if we have not found one yet
    if not GrabShape then
        local _, _, hit_pos, _, hit_entity = Physics.raycast(origin, dir, 5.0)

        if Ref.is_valid(hit_entity) then
            GrabShape = VoxelShape.get_component_for_entity(hit_entity)
            local node = Node.get_component_for_entity(hit_entity)
            GrabPosLS = Node.to_local_space(node, hit_pos)
            GrabDistance = Math.vec_distance(origin, hit_pos)
        end
    end

    -- Apply force to the grabbed shape
    if GrabShape then
        -- Calculate the target for our grabbed shape
        local grab_target_ws = Math.vec_add(origin, Math.vec_scale(GrabDistance, dir))

        -- Compute the current position of the grab in world coordinates
        local entity = VoxelShape.get_entity(GrabShape)
        local node = Node.get_component_for_entity(entity)
        local grab_pos_ws = Node.to_world_space(node, GrabPosLS)

        -- Draw a line from the grab position to the target
        DebugGeometry.draw_line(grab_pos_ws, grab_target_ws, Vec4(0.5, 0.5, 0.5, 0.5), true)

        -- Compute vector pointing from the current position of the grab to the target
        local to_grab_pos = Math.vec_sub(grab_target_ws, grab_pos_ws)
        local dist_to_grab_pos = Math.vec_length(to_grab_pos)

        if dist_to_grab_pos > 0.0001 then
            to_grab_pos = Math.vec_scale(1.0 / dist_to_grab_pos, to_grab_pos)
        end

        -- Compute force vector to move the grabbed shape to the target position
        local scale_dist = 1.0
        local force_factor = 1000.0 * delta_t / scale_dist
        -- Force is attenuated (stronger if the grabbed shape is farther away)
        local force = Math.vec_scale(Math.min(dist_to_grab_pos, scale_dist) * force_factor, to_grab_pos)

        -- Finally apply the force
        VoxelShape.apply_force_at_local_position(GrabShape, force, GrabPosLS)
    end
end

-- Simply resets the current grabbed shape
function EndGrabVoxelShape()
    GrabShape = nil
end

-- Handles character movement
function UpdateCharacter(delta_t)
    -- Fetch player and his character controller
    local player = Entity.find_first_entity_with_name("player")
    local player_cct = CharacterController.get_component_for_entity(player)
    local player_node = Node.get_component_for_entity(player)
    local grounded = CharacterController.is_grounded(player_cct)

    -- Simple water handling
    local water_plane = Entity.find_first_entity_with_name("water_plane")
    local water_plane_node = Node.get_component_for_entity(water_plane)
    local in_water = Node.get_world_position(player_node).y < Node.get_world_position(water_plane_node).y

    if not WasInWater and in_water then
        Sound.play_sound_effect("sp_water_splash")
        WasInWater = true
    elseif not in_water then
        WasInWater = false
    end

    if not grounded then
        if not TimeAirborne then
            TimeAirborne = 0.0
        end
        TimeAirborne = TimeAirborne + delta_t
    else
        TimeAirborne = 0.0
    end

    -- Fetch camera controller
    local cam = Entity.find_first_entity_with_name("game_camera")
    local cam_ctrl = CameraController.get_component_for_entity(cam)

    -- Apply gravity
    if not grounded and not in_water then
        PlayerVelocity.y = PlayerVelocity.y - delta_t * 9.81
    elseif PlayerVelocity.y < 0.0 then
        PlayerVelocity.y = PlayerVelocity.y * 0.999
    end

    -- Apply mouse movement
    local mouse_pos_rel = Input.get_mouse_pos_relative()
    local target_orientation = CameraController.get_target_euler_angles(cam_ctrl)
    target_orientation.x = target_orientation.x + mouse_pos_rel.y * 0.01
    target_orientation.y = target_orientation.y - mouse_pos_rel.x * 0.01

    -- WASD movement
    local mov_vec_local = Vec3(0.0)
    if Input.get_key_state(Key.kW, 0) == KeyState.kPressed then
        mov_vec_local.z = mov_vec_local.z + 1.0
    end
    if Input.get_key_state(Key.kS, 0) == KeyState.kPressed then
        mov_vec_local.z = mov_vec_local.z - 1.0
    end
    if Input.get_key_state(Key.kD, 0) == KeyState.kPressed then
        mov_vec_local.x = mov_vec_local.x - 1.0
    end
    if Input.get_key_state(Key.kA, 0) == KeyState.kPressed then
        mov_vec_local.x = mov_vec_local.x + 1.0
    end

    local mov_vec_len = Math.vec_length(mov_vec_local)
    if mov_vec_len > 0.0 then
        mov_vec_local = Math.vec_scale(1.0 / mov_vec_len, mov_vec_local)

        if not FootstepsSound and grounded then
            FootstepsSound = Sound.play_sound_effect("sp_footsteps")
        elseif FootstepsSound and TimeAirborne > 0.2 then
            Sound.stop_sound_effect(FootstepsSound)
            FootstepsSound = nil
        end
    else
        if FootstepsSound then
            Sound.stop_sound_effect(FootstepsSound)
            FootstepsSound = nil
        end
    end

    -- Apply speed
    local speed = 8.0
    mov_vec_local = Math.vec_scale(speed, mov_vec_local)

    local move_vec_global = Math.quat_rotate(Math.quat_from_euler_angles(Vec3(0.0, target_orientation.y, 0.0)),
        mov_vec_local)
    PlayerVelocity.x = move_vec_global.x
    PlayerVelocity.z = move_vec_global.z

    -- Jumping
    if (TimeAirborne < 0.1 or in_water) and Input.get_key_state(Key.kSpace, 0) == KeyState.kPressed and not Jumped then
        PlayerVelocity.y = 5.0
        Jumped = true
    elseif Input.get_key_state(Key.kSpace, 0) == KeyState.kReleased then
        Jumped = false
    end

    -- Update the camera controller
    CameraController.set_target_euler_angles(cam_ctrl, target_orientation)
    -- And finally move the player
    CharacterController.move(player_cct, PlayerVelocity)
end

---Called once when the script component becomes active.
---@param entity Ref The ref of the entity the script component is attached to.
function OnActivate(entity)
    -- Fetch player and camera
    local player = Entity.find_first_entity_with_name("player")
    local player_node = Node.get_component_for_entity(player)
    local cam = Entity.find_first_entity_with_name("game_camera")
    local cam_ctrl = CameraController.get_component_for_entity(cam)

    -- Set up camera controller
    CameraController.set_target_node(cam_ctrl, player_node)
    PlayerVelocity = Vec3(0.0)
end

--- Called every frame.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Tick(entity, delta_t)
    -- Draw the logo
    UI.draw_image("splash", Vec2(0.99, 0.01), Vec2(0.2, -1.0), Vec4(1.0), Vec2(1.0, 0.0))

    -- Spawn a grenade
    if Input.get_key_state(Key.kMouseLeft, 0) == KeyState.kPressed then
        if not GrenadeSpawned then
            SpawnPrefab("grenade", 2.0, 100.0, 1.0)
            GrenadeSpawned = true
        end
    else
        GrenadeSpawned = false
    end

    -- Spawn a mage ragdoll
    if Input.get_key_state(Key.kE, 0) == KeyState.kClicked then
        SpawnPrefab("mage_ragdoll", 2.0, 900.0, 1.0)
    end

    -- Try to grab a voxel shape in the scene
    if Input.get_key_state(Key.kMouseRight, 0) == KeyState.kPressed then
        GrabVoxelShape(delta_t)
    else
        EndGrabVoxelShape()
    end

    UpdateCharacter(delta_t)
end

--- Called once when the script component becomes inactive.
---@param entity Ref The ref of the entity the script component is attached to.
function OnDeactivate(entity)
end

--- Event callback
---@param entity Ref The ref of the entity the script component is attached to.
---@param events table List of all events.
function OnEvent(entity, events)
    ExplosionMask = {}
    local num_impact_sounds = 0

    for i = 1, #events do
        local e = events[i]
        if e.type == "Contact" then
            -- Check if a grenade hit something
            local grenade_node = nil
            if Ref.is_valid(e.data.entity0) then
                if Entity.get_name(e.data.entity0) == "grenade" then
                    grenade_node = Node.get_component_for_entity(e.data.entity0)
                end
            end
            if not grenade_node and Ref.is_valid(e.data.entity1) then
                if Entity.get_name(e.data.entity1) == "grenade" then
                    grenade_node = Node.get_component_for_entity(e.data.entity1)
                end
            end

            -- Play impact effects for everything but grenades
            local strength = Math.vec_length(e.data.impulse)
            if strength > 0.5 and num_impact_sounds < 3 then
                local impact_sound = Sound.play_sound_effect("sp_ground_hit")
                Sound.set_position(impact_sound, e.data.pos)
                num_impact_sounds = num_impact_sounds + 1
            end

            if grenade_node and not ExplosionMask[Ref.get_id(grenade_node)] then
                -- Apply damage
                World.radius_damage(e.data.pos, 1.0, true)
                ParticleSystem.spawn_particle_emitter("explosion", e.data.pos, 0.1, true)

                local explosion_sound = Sound.play_sound_effect("sp_explosion")
                Sound.set_position(explosion_sound, e.data.pos)

                -- And destroy the grenade
                Node.destroy(grenade_node)

                ExplosionMask[Ref.get_id(grenade_node)] = true
            end
        end
    end
end
