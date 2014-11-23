require "res/lib/metallic-crow"

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

function script_begin()
	stage00 = Stage:new(
		"res/common/world.json", 
		"res/common/choice.json", 
		"res/common/collision.json", 
		"res/common/subtitle.json")	
  
  	stage00:collision(true, "Hero", "Ensign", function() print "Hero - Ensign - Begin" end)

  	timer = Timer:new(stage00, script_end, 1, 0)

  	stage00:resume()
  	stage00:show()
end