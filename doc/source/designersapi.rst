.. _designersapi:

.. highlight:: lua

**************
Designers' API
**************

This section describes the Application Programming Interface exposed by the
Metallic Crow Story Engine for creating stories.  The designer creates a 
story as a series of scripts which correspond roughly to chapters or scenes 
within a story.

The interface provided by the engine is in the Lua scripting language.

Timer
=====

Timers allow the designer to trigger a specified Lua function with name 
call() after a certain time interval.

.. function:: timer_load(call, interval, loops)

Start a new timer.  After interval seconds invoke call(), repeat loops times.

.. function:: timer_free(call)

Release the timer to function call()


Subtitle
========

Subtitle allows the designer to present subtitle text on to the screen

.. function:: subtitle_text(text)

Display text

.. function:: subtitle_light(r, g, b)

Modulate the colour of displayed text

Stage
=====

A stage is a physical world for actors to play out the story, as well as the
fonts and graphics for choices and subtitles

.. function:: stage_nominate(name)

Nominate the stage called name to be the currently displayed stage

.. function:: stage_load(name, world_file, choice_file, collision_file, subtitle_file)

Load a stage designated name from the supplied files

.. function:: stage_free(name)

Free the stage called name

.. function:: stage_light(name, r, g, b)

Modulate the ambient lighting in the stage called name to be (r, g, b) 

.. function:: stage_pause(name)

Pause the stage called name

.. function:: stage_resume(name)

Resume the stage called name

Set
===

Set items are simple props, background items or foreground items.

.. function:: set_load(name, file)

load a prop from file and give it specified name

.. function:: set_free(name)

free the props called name

.. function:: set_light(name, r, g, b)

set the ambient lighting of props called name to (r, g, b)

Screen
======

A screen is a repeating surface used as a backdrop or for foreground effects.

.. function:: screen_load(name, file)

.. function:: screen_free(name)

.. function:: screen_light(name, r, g, b)

Fade
====

Fade provides the designer with a way to transitions between scenes.

.. function:: fade_up(period)

.. function:: fade_down(period)

Collision
=========

Collision allows the designer to trigger events when groups of objects touch.

.. function:: collision_link(group_a, group_b)

.. function:: collision_begin(group_a, group_b, call)

.. function:: collision_end(group_a, group_b, call)

Choice
======

Choice allows the player to make decisions that can be used to trigger events
in the story.

.. function:: choice(up, down, left, right, interval)


Actor
=====

Actors are the primary characters of a story.  They constitue a physical body,
facial and body expressions, and the ability to move.

.. function:: actor_load(name, file)

Load a new actor from file and add it to the group name 

.. function:: actor_free(name)

Free all actors called name

.. function:: actor_body(name, expression)

Change the body expression of all actors called name to expression

.. function:: actor_eyes(name, expression)

Change the eyes expression of all actors called name to expression

.. function:: actor_mouth(name, expression)

Change the mouth expression of all actors called name to expression

.. function:: actor_nominate(name)

Nominate the first actor found called name to be the hero

.. function:: actor_position(name, x, y)

Move all actors called name to position (x, y)

.. function:: actor_velocity(name, u, v)

Change the velocity of all actors called name to (u, v)

.. function:: actor_force(name, f, g)

Change the continuous force applied to all actors called name to (f, g)

.. function:: actor_impulse(name, i, j)

Apply an impulse of (i, j) to all actors called name

.. function:: actor_up(name)

Give the Up movement command to all actors called name

.. function:: actor_down(name)

Give the Down movement command to all actors called name

.. function:: actor_left(name)

Give the Left movement command to all actors called name

.. function:: actor_right(name)

Give the Right movement command to all actors called name

View
====

View controls the camera through which the story is viewed.

.. function:: view_add_actor(name)

.. function:: view_actor(name)

.. function:: view_point(x, y)

.. function:: view_zoom(z)

Script
======

Script is concerned with functionality of the script in the context of the 
story as a whole.

.. function:: script_initialise()

This special function, along with script_begin() must be provided by the
designer.  script_initialise() is the function initially called on first
loading the script.  

.. function:: script_begin()

This special function, along wih script_initialise() must be provided by the
designer.  script_begin() is the function called when the script begins
execution.

.. function:: script_end()

This function is provided to inform the game engine that the current script
has ended and the story should move on to the next script.