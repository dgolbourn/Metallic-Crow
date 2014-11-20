Timer = {}

function Timer:finalise()
	timer_free(self.stage, self.name)
end

function Timer:new(stage, command, interval, loops)
	if(self.number == nil) then
		self.__gc = self.finalise
		self.__index = self
		self.number = 0
	end
	self.number = self.number + 1

	object = {}
	object.name = tostring(self.number)
	setmetatable(object, self)
	object.stage = stage.name
	timer_load(object.stage, object.name, command, interval, loops)
	return object
end
