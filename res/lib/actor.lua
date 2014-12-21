Actor = {}

function Actor:finalise()
	actor_free(self.stage, self.name)
end

function Actor:body(expression, facing)
	if(expression == nil) then
		expression = ""
	end
	if(facing == nil) then
		facing = 0
	end
	actor_body(self.stage, self.name, expression, facing)
end

function Actor:eyes(expression)
	actor_eyes(self.stage, self.name, expression)
end

function Actor:mouth(expression)
	actor_mouth(self.stage, self.name, expression)
end

function Actor:view()
	view(self.stage, self.name)
end

function Actor:view_also()
	view_add(self.stage, self.name)
end

function Actor:position(x, y)
	actor_position(self.stage, self.name, x, y)
end

function Actor:velocity(u, u)
	actor_velocity(self.stage, self.name, u, v)
end

function Actor:force(f, g)
	actor_force(self.stage, self.name, f, g)
end

function Actor:impulse(i, j)
	actor_impulse(self.stage, self.name, i, j)
end

function Actor:modulation(r, g, b, a)
	actor_modulation(self.stage, self.name, r, g, b, a)
end

function Actor:new(stage, actor, actors)
	if(self.number == nil) then
		self.__gc = self.finalise
		self.__index = self
		self.number = 0
	end
	
	object = {}
	if(actors == nil) then
		self.number = self.number + 1
		object.name = tostring(self.number)
	else
		object.name = actors.name
	end
	setmetatable(object, self)
	object.stage = stage.name
	actor_load(object.stage, object.name, actor)
	return object
end
