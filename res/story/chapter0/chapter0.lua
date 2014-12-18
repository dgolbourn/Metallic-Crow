require "res/lib/metallic-crow"

left_facing = false

function control(x, y)
  hero:force(x * 5000, y * -5000)
  if (x < 0) and not left_facing then
    left_facing = true
    hero:body(nil, -1)
  elseif (x > 0) and left_facing then
    left_facing = false
    hero:body(nil, 1)
  end
end

function choice_up()
end

function choice_down()
end

function choice_left()
end

function choice_right()
end

function choice_timer()
end

function script_initialise()
end

function blah()
  print "start"
end

function blahb()
  print "end"
end

function script_begin()
  stage00 = Stage:new("res/common/world.json", "res/common/choice.json", "res/common/collision.json", "res/common/subtitle.json")	
  stage00:collision(true, "Hero", "World", blah)
  stage00:collision(false, "Hero", "World", blahb)

  --timer = Timer:new(stage00, script_end, 5, 0)

  sky = Screen:new(stage00, "res/story/chapter0/sky.json")

  --fade_up(30)
  --fade_end(function() print "hi" end)
  --fade_end(script_end)
  --collectgarbage()
  
  music_load("1", "test", "res/story/chapter0/music.json")
  music_play("1", "test")

  stage00:subtitle("test")

  stage00:choice("up", "down", "", "...", 3)

  arena1 = Actor:new(stage00, "res/story/chapter0/arena1.json")

  hero = Actor:new(stage00, "res/story/chapter0/hero.json")

  hero:body(nil, -1)
  hero:view()

  stage00:show()
  stage00:resume()
end