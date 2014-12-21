Stage = {}

function Stage:finalise()
	stage_free(self.name)
end

function Stage:show()
	stage_nominate(self.name)
end

function Stage:ambient(r, g, b)
	stage_ambient(self.name, r, g, b)
end

function Stage:pause()
	stage_pause(self.name)
end

function Stage:resume()
	stage_resume(self.name)
end

function Stage:collision(begin, group_a, group_b, command)
	if(begin == true) then
		collision_begin(self.name, group_a, group_b, command)
	else
		collision_end(self.name, group_a, group_b, command)	
	end
end

function Stage:choice(up, down, left, right, interval)
	choice(self.name, up, down, left, right, interval)
end

function Stage:choice_modulation(choice, r, g, b, a)
	if(choice == "up") then
		choice_up_modulation(self.name, r, g, b, a)
	elseif(choice == "down") then
		choice_down_modulation(self.name, r, g, b, a)
	elseif(choice == "left") then
		choice_left_modulation(self.name, r, g, b, a)
	elseif(choice == "right") then
		choice_right_modulation(self.name, r, g, b, a)
	end
end

function Stage:text(text)
	subtitle_text(self.name, text)
end

function Stage:text_modulation(r, g, b, a)
	subtitle_modulation(self.name, r, g, b, a)
end

function Stage:view(z)
	view_zoom(self.name, z)
end

function Stage:new(world, choice, collision, subtitle)
	if(self.number == nil) then
		self.__gc = self.finalise
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
