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

Entity.load()
AnimationSystem.load()
World.load()
Node.load()
Math.load()

function OnActivate(entity)
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

  -- Keep track of time
  Time = 0.0
end

function Tick(entity, delta_t)
  Time = Time + delta_t

  -- Vary play speed
  local speed = Math.sin(Time) + 1.0
  AnimationSystem.set_play_speed(AnimCircle, 1.0 * speed)
  AnimationSystem.set_play_speed(AnimWalk, 2.0 * speed)

  -- Blend out walk animation when stopping
  AnimationSystem.set_blend_weight(AnimWalk, Math.min(speed / 0.25, 1.0))
end

function OnDeactivate(entity)
  AnimationSystem.stop_animation(AnimCircle)
  AnimationSystem.stop_animation(AnimWalk)
end
