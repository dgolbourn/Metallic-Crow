Sound = {}

function Sound:finalise()
	sound_free(self.stage, self.name)
end

function Sound:play()
	sound_play(self.stage, self.name)
end

function Sound:stop()
	sound_end(self.stage, self.name)
end

function Sound:new(stage, sound)
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
	sound_load(object.stage, object.name, sound)
	return object
end
