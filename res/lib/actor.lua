Actor = {}

function Actor:finalise()
	actor_free(self.stage, self.name)
end

function Actor:light(r, g, b)
	actor_light(self.stage, self.name, r, g, b)
end

function Actor:body(expression)
	actor_body(self.stage, self.name, expression)
end

function Actor:eyes(expression)
	actor_eyes(self.stage, self.name, expression)
end

function Actor:mouth(expression)
	actor_mouth(self.stage, self.name, expression)
end

function Actor:lead()
	actor_nominate(self.stage, self.name)
end

function Actor:view(z)
	view_actor(self.stage, self.name)
	if(z ~= nil) then
		view_zoom(self.stage, z)
	end
end

function Actor:view_also(z)
	view_add_actor(self.stage, self.name)
	if(z ~= nil) then
		view_zoom(self.stage, z)
	end
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

function Actor:up()
	actor_up(self.stage, self.name)
end

function Actor:down()
	actor_down(self.stage, self.name)
end

function Actor:left()
	actor_left(self.stage, self.name)
end

function Actor:right()
	actor_right(self.stage, self.name)
end

function Actor:new(stage, actor, troupe)
	if(self.number == nil) then
		self.__gc = self.finalise
		self.__index = self
		self.number = 0
	end
	
	object = {}
	if(troupe == nil) then
		self.number = self.number + 1
		object.name = tostring(self.number)
	else
		object.name = troupe.name
	end
	setmetatable(object, self)
	object.stage = stage.name
	actor_load(object.stage, object.name, actor)
	return object
end
