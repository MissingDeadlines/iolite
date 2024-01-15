.. _animation_system:

Animation system
================

The animation system can animate hierarchies of entities and shapes, e.g., a character or a chest the player should be able to open. Animations can also be layered and blended hierarchically, making it possible to, for example, apply a hit animation, which only affects the upper body, on top of a running walk cycle.

Preparing entities for animation
--------------------------------

If you want to prepare a hierarchy of entities and shapes for animating, all entities participating in the animation must be tagged with an *Animation Node* component. *Animation Nodes* only have a single property that assigns each node a symbolical name, which is then used to map the animation data to the runtime transformation data.

The following screenshot shows a character with a simple rig defined by tagging each of the participating entities using an *Animation Node* component:

.. image:: /_static/images/animation_nodes.jpg

As shown in the screenshot above, the hierarchy defined by the *Animation Nodes* is visualized in the editor. Nodes participating in the animation (which which thus own an *Animation Node* component) are visualized in green. Nodes which are not directly animated, are visualized in gray. 

After this process, playing a matching animation on the entity hierarchy is possible.

Authoring animations
--------------------

Animations have to be authored in an external tool like Blender. To ease this process and when the entity hierarchy has been correctly tagged with *Animation Node* components, depicted in the previous section, IOLITE can export the resulting "rig" in the glTF file format.

To export a rig animation rig, click the root entity of the prepared hierarchy and head to ``[Import and Export]``, select ``[Export Selected Animation Rig]``, and finally, choose a destination and confirm.

Heading over to Blender, the imported animation rig for our example character looks like this:

.. image:: /_static/images/animation_rig_in_blender.jpg

When you are happy with your animation, export it as a glTF binary to the ``<data source>/media/animations`` directory and ensure it's suffixed with ``.glb``.

.. note:: The exported animation rig is currently only sparsely visualized by the bounds of each of the entities and their according shapes. To further improve this workflow in a future release, it will be possible to export a mesh representation of the shapes along with the rig.

Playing animations via the editor
---------------------------------

The simplest way to test an animation is via the editor. To play an animation, select the root entity of the hierarchy you want to animate. Right-click on the entity in the *World Inspector*, select ``[Animations] => [Play Animation]``, and choose an animation that fits the given hierarchy of entities. You can also use the ``[Animations]`` context menu to loop, layer, and stop animations.

Working with the animation system
---------------------------------

When working on your game, you can play, layer, suspend, and stop animations via the native C or Lua API.

Here's a simple snippet that showcases the usage of the animation system via a Lua script:

.. code-block:: lua

  Node.load()
  Math.load()
  AnimationSystem.load()

  function OnActivate(entity)
    -- The time passed since this script was activated
    Time = 0.0

    -- Retrieve the node of the entity this script is attached to
    local n = Node.get_component_for_entity(entity)

    -- Prepare the description for the animation we want to play
    local desc = AnimationSystem.AnimationDesc()
    -- The name of the animation to play
    desc.animation_name = "anination_a"
    -- Loop the animation until we stop it manually
    desc.looping = true

    -- Play the animation on the root node node of the hierarchy 
    -- we've retrieved earlier
    AnimInstanceA = AnimationSystem.play_animation(n, desc)

    -- Prepare an animation to layer on top of the base animation
    desc.animation_name = "animation_b"
    desc.looping = true
    -- Player the animation at twice the speed
    desc.play_speed = 2.0
    -- Blend it out initially so we can blend it in later on
    desc.blend_weight = 0.0

    -- Play the second animation
    AnimInstanceB = AnimationSystem.play_animation(n, desc)
  end

  function OnTick(entity)
    -- Advance time
    Time = Time + delta_t
    -- As an example, blend the layered animation in and out
    AnimationSystem.set_blend_weight(AnimInstanceB, Math.sin(Time) * 0.5 + 0.5)
  end

  function OnDeactivate(entity)
    AnimationSystem.stop_animation(AnimInstanceA)
    AnimationSystem.stop_animation(AnimInstanceB)
  end

In the example, we're playing a base animation on the node of the entity the script is attached to. In addition, we're playing an additional animation on top, which is initially blended out and won't affect the base layer animation. Over time, using the sine function, we're gradually blending in and out of the top layer animation. When the component is deactivated, either when the entity itself gets destroyed or the component itself, we're stopping both animations.

Working with the animation system via the native C API works analogously.