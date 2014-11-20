Screen = {}

function Screen:finalise()
	screen_free(self.stage, self.name)
end

function Screen:light(r, g, b)
	screen_light(self.stage, self.name, r, g, b)
end

function Screen:new(stage, screen, setting)
	if(self.number == nil) then
		self.__gc = self.finalise
		self.__index = self
		self.number = 0
	end
	
	object = {}
	if(setting == nil) then
		self.number = self.number + 1
		object.name = tostring(self.number)
	else
		object.name = setting.name
	end
	setmetatable(object, self)
	object.stage = stage.name
	screen_load(object.stage, object.name, screen)
	return object
end
