Set = {}

function Set:finalise()
	set_free(self.stage, self.name)
end

function Set:light(r, g, b)
	set_light(self.stage, self.name, r, g, b)
end

function Set:new(stage, set, scenery)
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
	set_load(object.stage, object.name, set)
	return object
end
