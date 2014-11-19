Stage = {}

function Stage:finalise()
	stage_free(self.name)
end

function Stage:show()
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

function Stage:collision_begin(group_a, group_b, command)
	collision_begin(self.name, group_a, group_b, command)
end

function Stage:collision_end(group_a, group_b, command)
	collision_end(self.name, group_a, group_b, command)
end

function Stage:choice(up, down, left, right, interval)
	choice(self.name, up, down, left, right, interval)
end

function Stage:subtitle(text, r, g, b)
	subtitle_text(self.name, text)
	if ((r ~= nil) and (g ~= nil) and (b ~= nil)) then
		subtitle_light(self.name, r, g, b)
	end
end

function Stage:view(x, y, z)
	view_point(self.name, x, y)
	view_zoom(self.name, z)
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
