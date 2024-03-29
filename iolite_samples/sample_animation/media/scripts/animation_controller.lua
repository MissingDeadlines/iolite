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

AnimationSystem.load()
Math.load()
Events.load()
Entity.load()
Node.load()

function OnActivate(entity)
  Events.register_event_listener(entity, "start_animations")
  Events.register_event_listener(entity, "stop_animations")
  Events.register_event_listener(entity, "update_animations")
end

function OnUserEvent(entity, events)
  for i = 1, #events do
    local e = events[i]
    if e.type == "start_animations" then
      local e = Entity.find_first_entity_with_name("knight")
      local n = Node.get_component_for_entity(e)

      local desc = AnimationSystem.AnimationDesc()
      desc.animation_name = "walk_circle"
      desc.looping = true

      -- Base animation that animates the knights root node around a circle
      AnimCircle = AnimationSystem.play_animation(n, desc)

      desc.animation_name = "walk"
      desc.looping = true
      desc.play_speed = 2.0

      -- Walk cycle layered on top
      AnimWalk = AnimationSystem.play_animation(n, desc)
    elseif e.type == "stop_animations" then
      if AnimCircle then
        AnimationSystem.stop_animation(AnimCircle)
        AnimCircle = nil
      end
      if AnimWalk then
        AnimationSystem.stop_animation(AnimWalk)
        AnimWalk = nil
      end
    elseif e.type == "update_animations" then
      local time = Variant.get_float(e.data.variants[1])
      local speed_factor = Variant.get_float(e.data.variants[2])

      -- Vary play speed
      local speed = Math.sin(time) * speed_factor + 1.0
      if AnimCircle then
        AnimationSystem.set_play_speed(AnimCircle, 1.0 * speed)
      end
      if AnimWalk then
        AnimationSystem.set_play_speed(AnimWalk, 2.0 * speed)
      end

      -- Blend out walk animation when stopping
      AnimationSystem.set_blend_weight(AnimWalk, Math.min(speed / 0.25, 1.0))
    end
  end
end

function OnDeactivate(entity)
  Events.unregister_event_listener(entity, "start_animations")
  Events.unregister_event_listener(entity, "stop_animations")
  Events.unregister_event_listener(entity, "update_animations")
end
