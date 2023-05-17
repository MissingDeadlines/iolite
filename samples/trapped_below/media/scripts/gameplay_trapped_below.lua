Input.load()
Physics.load()
World.load()
Math.load()
Log.load()
DebugGeometry.load()
Node.load()
UI.load()
Light.load()
Entity.load()
Math.load()
Noise.load()
VoxelShape.load()
Sound.load()

BrushSize = 0.1
CursorMode = 0
TimePassed = 0.0

function AnimateLights()
    local light_ceil_e = Entity.find_first_entity_with_name("light_ceiling")
    local light_ceil_l = Light.get_component_for_entity(light_ceil_e)

    local light_ceil_intens = 50.0
    if not StateLightCeiling then
        light_ceil_intens = 0.0
    end

    light_ceil_intens = light_ceil_intens - Math.pow(Noise.simplex(Vec4(TimePassed, 0.0, 0.0, 0.0)), 2.0) *
                            light_ceil_intens

    Light.set_property(light_ceil_l, "Intensity", Variant.from_float(light_ceil_intens))
end

function DrawCursor()
    local cursor = "cursor"
    if CursorMode == 1 then
        cursor = "cursor_grab"
    end

    UI.draw_image(cursor, Input.get_mouse_pos_viewport(), Vec2(-1.0, 0.025), Vec4(1.0), Vec2(0.0, 0.0))
end

function CalcGrabPos(origin, dir, height)
    local plane_normal = Vec3(0.0, 1.0, 0.0)

    local denom = Math.vec_dot(dir, plane_normal)
    local center = Vec3(0.0, height, 0.0)
    local t = Math.vec_dot(Math.vec_sub(center, origin), plane_normal) / denom

    return Math.vec_add(origin, Math.vec_scale(t, dir))
end

function GrabVoxelShape(delta_t)
    local origin, dir = World.calc_mouse_ray()

    if not GrabShape then
        local _, _, hit_pos, _, hit_entity = Physics.raycast(origin, dir, 100.0)

        if Ref.is_valid(hit_entity) then
            GrabShape = VoxelShape.get_component_for_entity(hit_entity)
            local node = Node.get_component_for_entity(hit_entity)
            GrabPosLS = Node.to_local_space(node, hit_pos)
            GrabHeight = hit_pos.y
            GrabHeightOffset = 0.0
        end
    end

    if Input.get_key_state(Key.kE, 0) == KeyState.kPressed then
        GrabHeightOffset = GrabHeightOffset + 1.0 * delta_t
    end

    if Input.get_key_state(Key.kQ, 0) == KeyState.kPressed then
        GrabHeightOffset = GrabHeightOffset - 1.0 * delta_t
    end

    if GrabShape then
        local grab_target_ws = CalcGrabPos(origin, dir, GrabHeight)

        grab_target_ws.y = grab_target_ws.y + GrabHeightOffset

        local entity = VoxelShape.get_entity(GrabShape)
        local node = Node.get_component_for_entity(entity)
        local grab_pos_ws = Node.to_world_space(node, GrabPosLS)

        local to_grab_pos = Math.vec_sub(grab_target_ws, grab_pos_ws)
        local dist_to_grab_pos = Math.vec_length(to_grab_pos)

        if dist_to_grab_pos > 0.0001 then
            to_grab_pos = Math.vec_scale(1.0 / dist_to_grab_pos, to_grab_pos)
        end

        local scale_dist = 5.0
        local scale_power = 1.0
        local force_factor = 5.0
        local force = Math.vec_scale(Math.pow(Math.min(dist_to_grab_pos, scale_dist) / scale_dist, scale_power) *
                                         force_factor, to_grab_pos)

        VoxelShape.apply_force_at_local_position(GrabShape, force, GrabPosLS)

        return true, grab_pos_ws, grab_target_ws
    end

    return false, _, _
end

function PushHint(hint, duration, start_function)
    if not Hints then
        Hints = {}
    end

    if #Hints > 0 then
        if Hints[1].duration == 0.0 then
            Hints[1].duration = 1.0
        end
    end

    table.insert(Hints, #Hints + 1, {
        hint = hint,
        alpha = 0.0,
        init = false,
        time_alive = 0.0,
        duration = duration,
        start_function = start_function
    })
end

function Ease(x)
    return 1.0 - Math.pow(1.0 - x, 3.0)
end

function UpdateHints(delta_t)
    if #Hints > 0 then
        local hint = Hints[1]

        if not hint.init then
            if hint.start_function then
                hint.start_function()
            end
            Sound.play_sound_effect("hint")
            hint.init = true
        end

        hint.time_alive = hint.time_alive + delta_t

        if hint.time_alive < hint.duration or hint.duration == 0.0 then
            hint.alpha = hint.alpha + delta_t
        elseif hint.duration > 0.0 then
            hint.alpha = hint.alpha - delta_t
        end

        hint.alpha = Math.clamp(hint.alpha, 0.0, 1.0)

        UI.push_font_scale(0.75)
        UI.draw_text(hint.hint, Vec2(0.5, 0.75), Vec2(0.0), Vec4(1.0, 1.0, 1.0, Ease(hint.alpha)), 1)
        UI.pop_font_scale()

        if hint.alpha <= 0.0 then
            table.remove(Hints, 1)
        end
    end
end

function DrawInteractorTitle(name)
    local pos = Input.get_mouse_pos_viewport()
    pos = Math.vec_add(pos, Vec2(0.02, 0.0))

    UI.push_font_scale(0.75)
    UI.draw_text(name, pos, Vec2(0.0), Vec4(1.0), 0)
    UI.pop_font_scale()
end

function LeftClicked()
    return Input.get_key_state(Key.kMouseLeft, 0) == KeyState.kClicked
end

function EndGrabVoxelShape()
    GrabShape = nil
end

function HandleInteraction(delta_t)
    -- Reset highlight initially
    World.highlight_node(Ref.create_invalid(), Vec4(0.0), true)

    local origin, dir = World.calc_mouse_ray()
    local hit, _, _, _, entity = Physics.raycast(origin, dir, 1000.0)
    if hit then
        local node = Node.get_component_for_entity(entity)
        local name = Entity.get_name(entity)

        if InteractorStates and InteractorStates[name] then
            if (name == "fuse_box") then
                World.highlight_node(node, Vec4(0.5, 0.5, 0.5, 0.5), true)
                DrawInteractorTitle("Fuse box")

                if LeftClicked() then
                    StateLightCeiling = true
                    InteractorStates[name] = false

                    Sound.play_sound_effect("click")
                    Sound.play_sound_effect("lamp_on")

                    PushHint("\"Thank goodness, it works! Finally, some light. Wait...where am I?\"", 4.0, nil)
                    PushHint("\"This doesn't make sense. How did I end up here? What's going on?\"", 4.0, nil)
                    PushHint("Goal: Find a way out of this room.", 0.0, nil)
                end
            end
        end
    end
end

function HandleInput(delta_t)
    if Input.get_key_state(Key.kMouseLeft, 0) == KeyState.kPressed then
        local grabbed, start_pos, end_pos = GrabVoxelShape(delta_t)
        if grabbed then
            CursorMode = 1
            DebugGeometry.draw_line(start_pos, end_pos, Vec4(0.5, 0.5, 0.5, 0.5), true)
        end
    else
        EndGrabVoxelShape()
        CursorMode = 0
    end

    HandleInteraction(delta_t)
end

function InitTutorial()
    InteractorStates = {
        fuse_box = true
    }
end

---Called once when the script component becomes active.
---@param entity Ref The ref of the entity the script component is attached to.
function OnActivate(entity)
    StateLightCeiling = false

    PushHint(
        "\"Whoa, where am I? Did I get kidnapped or something? This is not good. I need to get out of here, and fast.\"",
        4.0)
    PushHint(
        "\"Ugh, it's freezing in here. I can barely see a thing. There's got to be a light switch or something. I can't believe I'm stuck in here like a rat in a trap.\"",
        4.0, nil)
    PushHint("Press and hold [LMB] to lift objects. Use in combination with [E]/[Q] to pull objects up/down.", 0.0,
        InitTutorial)

    SoundBackground = Sound.play_sound_effect("basement_ambience")
    Sound.play_sound_effect("breathing")
end

--- Called every frame.
---@param entity Ref The ref of the entity the script component is attached to.
---@param delta_t number The time (in seconds) passed since the last call to this function.
function Tick(entity, delta_t)
    TimePassed = TimePassed + delta_t

    AnimateLights()
    UpdateHints(delta_t)
    DrawCursor()
    HandleInput(delta_t)
end

--- Called once when the script component becomes inactive.
---@param entity Ref The ref of the entity the script component is attached to.
function OnDeactivate(entity)
    Sound.stop_sound_effect(SoundBackground)
end

--- Event callback
---@param entity Ref The ref of the entity the script component is attached to.
---@param events table List of all events.
function OnEvent(entity, events)
    for i = 1, #events do
        local e = events[i]
        if e.type == "Contact" then
            local strength = Math.vec_length(e.data.impulse)
            if strength > 0.01 then
                Sound.play_sound_effect("ground_hit")
            end
        end
    end
end
