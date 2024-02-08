.. _sound_system:

Sound system
============

The sound system allows you to play sounds and music.

.. note:: The sound system currently does not feature a user interface and has to be configured by manually authoring JSON files.
   
Authoring audio files
---------------------
   
.. _Audacity: https://www.audacityteam.org/

The sound system supports **stereo** OGG and WAV audio files with a **fixed sample rate of 44100 Hz**.  The sound system searches for audio files in the ``media/sounds`` directory in any of the available `data sources <data_sources>`_.

.. note:: `Audacity`_ is a great free audio editor, which can be used for creating audio files in the right format.

.. warning:: Please make sure to use the right sample rate and number channels. Neither mono nor audio files with a different sample rate than 44100 Hz are supported.

Preparing sound effects
-----------------------

After preparing your audio files, you need to create the appropriate sound effects. A single sound effect comprises one or multiple audio samplers, which reference the actual audio files on disk.

To create a sound effect, create a ``my_sound_effect.effect.json`` file in the ``media/sounds`` directory in a `data source <data_sources>`_ of your choice, right next to your audio files. The effect file has the following format:

.. code-block:: json

    {
      "my_sound_effect": {
        // The member parameters of your effect as
        // as depicted below
        "audio_samplers": [
            {
              // Parameters for a single audio sampler as
              // depicted below
            }
        ]
      }
    }
   
A single effect supports the following parameters:
   
audio_samplers ``Array of audio samplers``
   The audio samplers for this effect. A single effect can play multiple samplers at the same time.
volume ``Float, optional``
   The volume for this sound effect. Use a value of ``0.0`` for a muted and a value of ``1.0`` for the loudest sound effect.
looping ``Boolean, optional``
   Set to ``true`` to automatically restart the effect instance when it has finished playing. Great for background music or ambient effects.
spatial ``Boolean, optional``
   Set to ``true`` for spatial/3D sound effects. Spatial effects have a position in 3D space and change their volume depending on the listener's position.
spatial_size ``Float, optional``
   The spatial size of the effect. If the distance to the listener is less than this value, the effect fades into a non-spatial sound effect.
dist_att_min ``Float, optional``
   The minimum distance attenuation volume. Sound effects will never get quieter than this volume value when attenuated based on the distance to the listener.
dist_att_power ``Float, optional``
   Specifies how quickly a sound effect is attenuated based on the distance to the listener.
dist_att_range ``Float, optional``
   The range world units in which the distance attenuation operates. Effects out of this range will be attenuated to the volume specified by the ``dist_att_min`` parameter.

Audio samplers support the following parameters:

file_names ``Array of strings``
   List of file names, including the file extension, of the audio files to play for this sampler. If more than one file name is provided, a random file is picked every time the effect is restarted.
volume ``Float, optional``
   The volume for this sampler. Behaves the same as the global volume for the effect.
timeline_offset ``Float, optional``
   The position on the effect's timeline in seconds when this sampler should start playing.
pitch ``Array of floats, optional``
   Randomly alters the pitch in the provided interval. Requires exactly two values in the array, which can be positive or negative. The first represents the minimum, and the second the maximum relative pitch change in cents.

Playing sound effects
---------------------

Sound effects can either be played via the `Lua scripting interface <api_lua>`_, the `C API <api_c>`_, or by placing a :ref:`Sound Component <sound_component>` in the scene.