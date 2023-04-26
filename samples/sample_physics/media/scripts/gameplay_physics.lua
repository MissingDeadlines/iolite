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

function UpdateCharacter(delta_t)
  -- Fetch player and his character controller
  local player = Entity.find_first_entity_with_name("player")
  local player_cct = CharacterController.get_component_for_entity(player)

  -- Fetch camera controller
  local cam = Entity.find_first_entity_with_name("game_camera")
  local cam_ctrl = CameraController.get_component_for_entity(cam)

  -- Apply gravity
  if not CharacterController.is_grounded(player_cct) then
    PlayerVelocity.y = PlayerVelocity.y - delta_t * 9.81
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
  end

  -- Apply speed
  local speed = 8.0
  mov_vec_local = Math.vec_scale(speed, mov_vec_local)

  local move_vec_global = Math.quat_rotate(Math.quat_from_euler_angles(Vec3(0.0, target_orientation.y, 0.0)), mov_vec_local)
  PlayerVelocity.x = move_vec_global.x
  PlayerVelocity.z = move_vec_global.z

  -- Jumping
  if CharacterController.is_grounded(player_cct) 
    and Input.get_key_state(Key.kSpace, 0) == KeyState.kPressed 
    and not Jumped then
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
end
