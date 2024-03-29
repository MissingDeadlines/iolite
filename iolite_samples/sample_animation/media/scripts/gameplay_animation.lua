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
FlipbookAnimation.load()
Events.load()

function OnActivate(entity)
  -- Keep track of time
  Time = 0.0

  -- Start the node-based animations for the knight via the event system
  -- See the separate "animation_controller.lua" script for details
  Events.post_event(entity, "start_animations")

  -- Set up flipbook animations, start with the dinosaur
  local e = Entity.find_first_entity_with_name("dino")
  local fb = FlipbookAnimation.get_component_for_entity(e)

  -- Play animation by setting the flags manually
  -- Flag bit 1: Play, Flag bit 2: Loop, Flag bit 3: Clear
  -- Bits: 0000 0111, Decimal: 7
  FlipbookAnimation.set_property(fb, "Flags", Variant.from_uint8(7))

  -- Set up the deer
  e = Entity.find_first_entity_with_name("deer")
  fb = FlipbookAnimation.get_component_for_entity(e)
  -- Play animation by using one of the helper functions instead of the flags
  FlipbookAnimation.play(fb)
end

function Tick(entity, delta_t)
  Time = Time + delta_t

  -- Adjust this to speed up the animations
  local speed_factor = 1.0

  -- Update the animation paramerters for the knight each frame via the event system
  -- See the separate "animation_controller.lua" script for details
  Events.post_event_with_payload(entity, "update_animations", {
    Variant.from_float(Time), Variant.from_float(speed_factor)
  })
end

function OnDeactivate(entity)
  -- Stop the animations of the knight
  -- See the separate "animation_controller.lua" script for details
  Events.post_event(entity, "stop_animations")
end
