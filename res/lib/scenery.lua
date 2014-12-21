Scenery = {}

function Scenery:finalise()
	scenery_free(self.stage, self.name)
end

function Scenery:modulation(r, g, b, a)
	scenery_modulation(self.stage, self.name, r, g, b, a)
end

function Scenery:new(stage, scenery, sceneries)
	if(self.number == nil) then
		self.__gc = self.finalise
		self.__index = self
		self.number = 0
	end

	object = {}
	if(sceneries == nil) then
		self.number = self.number + 1
		object.name = tostring(self.number)
	else
		object.name = sceneries.name
	end
	setmetatable(object, self)
	object.stage = stage.name
	scenery_load(object.stage, object.name, scenery)
	return object
end
