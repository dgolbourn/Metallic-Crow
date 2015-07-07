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

Start a new timer called ``name`` associatd with ``stage``.  After ``interval`` seconds call ``command()``, repeat ``loops`` times.

.. function:: timer_free(stage, name)

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

.. function:: stage_nominate(name)

Choose ``name`` to be the displayed stage.

.. function:: stage_load(name, configuration)

Load ``name`` with the provided configuration table.

.. function:: stage_free(name)

Free the stage.

.. function:: stage_modulate(name, r, g, b)

Set the ambient lighting.

.. function:: stage_pause(name)

Pause the stage.

.. function:: stage_resume(name)

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

.. function:: collision_begin(stage, name_a, name_b, command)

Register ``command`` to be called when ``name_a`` beings a collision with ``name_b``

.. function:: collision_end(stage, name_a, name_b, command)

Register ``command`` to be called when ``name_a`` ends its collision with ``name_b``


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

.. function:: actor_load(stage, name, configuration)

Load an actor ``name`` using the given configuration table. 

.. function:: actor_free(stage, name)

Free the actor.

.. function:: actor_body(stage, name, expression)

Change the body expression of the actor.

.. function:: actor_eyes(stage, name, expression)

Change the eyes expression of the actor.

.. function:: actor_mouth(stage, name, expression)

Change the mouth expression of the actor.

.. function:: actor_position(stage, name, x, y)

Move the actor.

.. function:: actor_velocity(stage, name, u, v)

Change the actor's velocity.

.. function:: actor_force(stage, name, f, g)

Change the continuous force applied to the actor.

.. function:: actor_impulse(stage, name, i, j)

Apply an impulse to the actor.

.. function:: actor_dilate(stage, name, dilation)

Change the animation speed of the actor.

.. function:: actor_modulation(stage, name, r, g, b, a)

Chage the actor's colour.

.. function:: actor_rotate(stage, name, angle)

Rotate the actor

View
====

View controls the location of the camera in its associated stage.

.. function:: view(stage, name)

Position the camera on the actor ``name``.

.. function:: view_add(stage, name)

Position the camera such that actor ``name`` is also in view.

.. function:: view_zoom(stage, zoom)

Change the zoom of the camera.

.. function:: view_rotate(stage, angle)

Rotate the camera.

Audio
=====

Sound effects and music are associated with a given stage and are paused and resumed when the stage is paused and resumed.  Also, they are only audible when the stage is displayed.  Sound refers to generally short sound effects, such as talking, explosions and footsteps, Music refers to longer sounds such as background music.  At any one time there can only be one piece of music playing but there can be many sound effects.

.. function:: sound_load(stage, name, configuration)

Load the sound as ``name`` using the given configuration table.

.. function:: sound_free(stage, name)

Free the sound.

.. function:: sound_play(stage, name, volume)

Play the sound.

.. function:: sound_end(stage, name)

Stop the sound.

.. function:: music_load(stage, name, configuration)

Load the music as ``name`` using the given configuration table.

.. function:: music_free(stage, name)

Free the music.

.. function:: music_play(stage, name, volume)

Play the music.

.. function:: music_end(stage, name)

Stop the music.


Joint
=====

A joint is a physics object that can be used to join two actors together.

.. function:: joint_load(stage, name, actor_a, actor_b, configuration)

Create a new joint between actor_a and actor_b using the given configuration table.

.. function:: joint_free(stage, name)

Remove the joint.
