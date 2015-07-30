.. _library:

.. highlight:: lua

*******
Library
*******

This section describes the Application Programming Interface of Metallic Crow.  The API is presented as a library in the Lua extension language.  As Lua is designed to support procedural programming, the API is presented in a procedural style.

To access these API calls, use ``require "metallic_crow"`` in your Lua script.  All of the functions described below are in the returned library table.


Script
======

A story is built up from many scripts.  Each script represents a non-recurring unit of your story. For example; a chapter, scene, episode, checkpoint, or act.  The game loads each script in sequence as defined in the configuration.  All story events and game logic occur in these scripts.  The game automatically saves a player's progress at the granularity of scripts.

.. function:: exit()

Call ``exit()`` to tell the engine that this script is complete.  The game will then move on to the next script.

Events
======

The library table contains several entries for callbacks.  These are called by the engine when a user event occurs.  By assigning your own functions to these entries in the table your script will be informed when the event occurs.

.. function:: main()

When the game decides to begin the script, it will call ``main()`` as the first event.  For example, this event may be the place where you choose to load game assets and invoke an initial story event.

.. function:: control(x, y)

When the player moves the controller, this event is called with the new direction of the controller.

.. function:: choice_up()

When the player chooses the **up** choice, this event is called.

.. function:: choice_down()

When the player chooses the **down** choice, this event is called.

.. function:: choice_left()

When the player chooses the **left** choice, this event is called.

.. function:: choice_right()

When the player chooses the **right** choice, this event is called.

.. function:: choice_timer()

When the choice timer elapses, this event is called.


Timer
=====

Timers allow you to trigger events after a time interval.  Timers are associated with a `stage`_ and are paused when the stage is paused.

.. function:: timer_load(stage, name, command, interval, loops)

Start a new timer managed by ``stage``.  After ``interval`` seconds call ``command()``, repeat ``loops`` times.  Returns a timer handle to use for identifying the timer in subsequent calls.

.. function:: timer_free(timer)

Halt the timer.


Subtitle
========

Subtitle allows you to display subtitle text.  Subtitles are associated with a `stage`_ and are only displayed when the stage is being displayed.

.. function:: subtitle_text(stage, text)

Display text.

.. function:: subtitle_modulate(stage, r, g, b)

Change the text colour.


Stage
=====

A stage is a physical world for actors to play out the story.  Its configuration also defines the style of fonts and graphics for choices and subtitles.  A stage represents a locale, room, level or zone.  For example, the interior and exterior of a house could be two stages that are part of the same script.  Multiple stages can be loaded and running at the same time, but only one can be displayed.  Calls such as `actor`_, `audio`_, `view`_, `timer`_, `subtitle`_ and `choice`_  are all inextricably linked to their stages. Entities associated with a stage are paused when their stage is paused, visible and audible when their stage is displayed and freed when their stage is freed.

.. function:: stage_nominate(stage)

Choose ``stage`` to be the displayed stage.

.. function:: stage_load(configuration)

Load stage with the provided configuration table.  Returns a stage handle to use for identifying the stage in subsequent calls.

.. function:: stage_free(stage)

Free the stage.

.. function:: stage_modulate(stage, r, g, b)

Set the ambient lighting.

.. function:: stage_pause(stage)

Pause the stage.

.. function:: stage_resume(stage)

Resume the stage.


Fade
====

Invoke a fade to allow transitions between scenes or for other dramatic effects.

.. function:: fade_up(period)

Fade up to clear over ``period`` seconds.

.. function:: fade_down(period)

Fade down to black over ``period`` seconds.

.. function:: fade_end(command)

Register ``command`` to be called when fade completes.


Collision
=========

Collisions are events that occur when two actors touch.

.. function:: collision_begin(stage, group_a, group_b, command)

Register ``command`` to be called when an actor associated with ``group_a`` beings a collision with an actor in ``group_b``

.. function:: collision_end(stage, group_a, group_b, command)

Register ``command`` to be called when an actor associated with ``group_a`` ends its collision with an actor in ``group_b``


Choice
======

Choice is a context menu that you can create to allow the player to make decisions in the story.  Each `stage`_ can have one choice menu active at a time.

.. function:: choice(stage, up, down, left, right, interval)

Display a choice menu with text for ``up``, ``down``, ``left`` and ``right`` choices, with a timeout choice triggering after ``interval`` seconds.

.. function:: choice_up_modulation(stage, r, g, b, a)

Set the colour of the **up** choice text 

.. function:: choice_down_modulation(stage, r, g, b, a)

Set the colour of the **down** choice text 

.. function:: choice_left_modulation(stage, r, g, b, a)

Set the colour of the **left** choice text 

.. function:: choice_right_modulation(stage, r, g, b, a)

Set the colour of the **right** choice text 


Actor
=====

Actors are the primary entities that make up a story.  They can have physics properties such as mass and velocity and can collide with one another.  They also have animation properties and can have multiple expressions and poses.  Actors are used to create many things, for example: static props, animated pieces of terrain, the ground that other actors stand on and the characters of the story.

.. function:: actor_load(stage, configuration)

Load an actor using the given configuration table. Returns an actor handle to use for identifying the actor in subsequent calls.

.. function:: actor_free(actor)

Free the actor.

.. function:: actor_body(actor, expression)

Change the body expression of the actor.

.. function:: actor_eyes(actor, expression)

Change the eyes expression of the actor.

.. function:: actor_mouth(actor, expression)

Change the mouth expression of the actor.

.. function:: actor_position(actor, x, y)

Move the actor.

.. function:: actor_velocity(actor, u, v)

Change the actor's velocity.

.. function:: actor_force(actor, f, g)

Change the continuous force applied to the actor.

.. function:: actor_impulse(actor, i, j)

Apply an impulse to the actor.

.. function:: actor_dilation(actor, dilation)

Change the animation speed of the actor.

.. function:: actor_modulation(actor, r, g, b, a)

Chage the actor's colour.

.. function:: actor_rotation(actor, angle)

Rotate the actor

.. function:: actor_scale(actor, scale)

Scale the actor

View
====

View controls the location of the camera in its associated stage.

.. function:: view(actor)

Position the camera on the actor ``actor``.

.. function:: view_add(actor)

Position the camera such that actor ``actor`` is also in view.

.. function:: view_zoom(stage, zoom)

Change the zoom of the camera.

.. function:: view_rotation(stage, angle)

Rotate the camera.

Audio
=====

Sound effects and music are associated with a given stage and are paused and resumed when the stage is paused and resumed.  Also, they are only audible when the stage is displayed.  Sound refers to generally short sound effects, such as talking, explosions and footsteps, Music refers to longer sounds such as background music.  At any one time there can only be one piece of music playing but there can be many sound effects.

.. function:: sound_load(stage, configuration)

Load the sound using the given configuration table.  Returns a sound handle to use for identifying the sound in subsequent calls.

.. function:: sound_free(sound)

Free the sound.

.. function:: sound_play(sound, volume)

Play the sound.

.. function:: sound_end(sound)

Stop the sound.

.. function:: music_load(stage, configuration)

Load the music using the given configuration table.  Returns a music handle to use for identifying the music in subsequent calls.

.. function:: music_free(music)

Free the music.

.. function:: music_play(music)

Play the music.

.. function:: music_end(music)

Stop the music.


Joint
=====

A joint is a physics object that can be used to join two actors together.

.. function:: joint_load(actor_a, actor_b, configuration)

Create a new joint between actor_a and actor_b using the given configuration table.  Returns a joint handle to use for identifying the joint in subsequent calls.

.. function:: joint_free(joint)

Remove the joint.
