Music = {}

function Music:finalise()
	music_free(self.stage, self.name)
end

function Music:play()
	music_play(self.stage, self.name)
end

function Music:stop()
	music_end(self.stage, self.name)
end

function Music:new(stage, music)
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
	music_load(object.stage, object.name, music)
	return object
end
