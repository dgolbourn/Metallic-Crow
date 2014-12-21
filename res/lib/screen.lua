Screen = {}

function Screen:finalise()
	screen_free(self.stage, self.name)
end

function Screen:modulation(r, g, b, a)
	screen_modulation(self.stage, self.name, r, g, b, a)
end

function Screen:new(stage, screen, screens)
	if(self.number == nil) then
		self.__gc = self.finalise
		self.__index = self
		self.number = 0
	end
	
	object = {}
	if(screens == nil) then
		self.number = self.number + 1
		object.name = tostring(self.number)
	else
		object.name = screens.name
	end
	setmetatable(object, self)
	object.stage = stage.name
	screen_load(object.stage, object.name, screen)
	return object
end
