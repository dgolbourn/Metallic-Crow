Stage = {}

function Stage:finalise()
	stage_free(self.name)
	print "finalise"
end

function Stage:nominate()
	stage_nominate(self.name)
end

function Stage:light(r, g, b)
	stage_light(self.name, r, g, b)
end

function Stage:pause()
	stage_pause(self.name)
end

function Stage:resume()
	stage_resume(self.name)
end

function Stage:new(world, choice, collision, subtitle)
	if(self.number == nil) then
		self.__gc = Stage.finalise
		self.__index = self
		self.number = 0
	end
	self.number = self.number + 1

	object = {}
	object.name = tostring(self.number)
	setmetatable(object, self)
	stage_load(object.name, world, choice, collision, subtitle)
	return object
end
