Joint = {}

function Joint:finalise()
	joint_free(self.stage, self.name)
end

function Joint:new(stage, joint, actor_a, actor_b, joints)
	if(self.number == nil) then
		self.__gc = self.finalise
		self.__index = self
		self.number = 0
	end

	object = {}
	if(joints == nil) then
		self.number = self.number + 1
		object.name = tostring(self.number)
	else
		object.name = joints.name
	end
	setmetatable(object, self)
	object.stage = stage.name
	joint_load(object.stage, object.name, joint, actor_a, actor_b)
	return object
end
