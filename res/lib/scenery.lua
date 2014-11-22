Scenery = {}

function Scenery:finalise()
	scenery_free(self.stage, self.name)
end

function Scenery:light(r, g, b)
	scenery_light(self.stage, self.name, r, g, b)
end

function Scenery:new(stage, set, scenery)
	if(self.number == nil) then
		self.__gc = self.finalise
		self.__index = self
		self.number = 0
	end

	object = {}
	if(scenery == nil) then
		self.number = self.number + 1
		object.name = tostring(self.number)
	else
		object.name = scenery.name
	end
	setmetatable(object, self)
	object.stage = stage.name
	scenery_load(object.stage, object.name, set)
	return object
end
