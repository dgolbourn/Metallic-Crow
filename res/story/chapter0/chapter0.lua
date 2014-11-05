function initialise()
end

function begin()
  stage_load(
	"scene00", 
	"res/common/world.json", 
	"res/common/choice.json", 
	"res/common/collision.json", 
	"res/common/subtitle.json")

  stage_nominate("scene00")
end