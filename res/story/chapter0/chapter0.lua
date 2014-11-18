require "res/lib/metallic-crow"

function script_initialise()
end

function script_begin()
	scene00 = Stage:new(
		"res/common/world.json", 
		"res/common/choice.json", 
		"res/common/collision.json", 
		"res/common/subtitle.json")	
  
  	scene00:nominate()

	script_end()
end