.. _animation_system:

Animation system
================

This section serves as a guide for the animation system, which can be used to play animations on shape/entity hierarchies.

.. note:: Animations have to be authored in an external tool that supports exporting animations like Blender. Support for authoring animations in-engine is planned for a future release.

Overview
--------

The animation system can animate hierarchies of entities and shapes, like, e.g., a character. Animations can also be layered and blended hierarchically, making it possible to, e.g., apply a hit animation, which only affects the upper body, on top of a running walk cycle.

Preparing an animation
----------------------

To prepare a hierarchy of entities and shapes for animating, all entities participating in the animation must be tagged with an *Animation Node* component. *Animation Nodes* only have a single property that assigns each node a symbolical name, which is then used to map the animation data to the runtime transformation data.

The following screenshot shows a character with a simple rig defined by tagging each of the participating entities using an *Animation Node* component:

.. image:: /_static/images/animation_nodes.jpg

Animations can then be authored in an external tool like Blender. To ease this process, IOLITE can export the rig as a glTF binary file. To export a rig animation rig, click the root entity of the prepared hierarchy and head to ``[Import and Export]``, select ``[Export Selected Animation Rig]``, and finally choose a destination, and confirm.

Heading over to Blender, the imported animation rig for our example character looks like this:

.. image:: /_static/images/animation_rig_in_blender.jpg

When your happy with your animation, export it as a glTF binary to the ``<data source>/media/animations`` directory and sure it's suffixed with ``.glb``.

.. note:: The exported animation rig is currently only sparsely visualized by the bounds of each of the entities and their according shapes. To further improve this workflow in a future release, it will be possible to export a mesh representation of the shapes along with the rig.

Testing animations in the editor
--------------------------------

The simplest way to test an animation is via the editor. To play an animation, select the root entity of the hierarchy you want to animate. Right-click on the entity in the *World Inspector*, select ``[Animations] => [Play Animation]``, and choose a fitting animation. You can also use the ``[Animations]`` context menu to loop, layer, and stop animations.

Working with the animation system
---------------------------------

When working on your game, you can play, layer, suspend, and stop animations via the native C or Lua API.