local MetallicCrow = require "metallic_crow"
local Load = require "mcl/load"

local Actor = {}

local function final(self)
  if self.data then
    actor_free(self.data)
  end
end

Actor.__gc = final
Actor.__index = Actor

function Actor:delete()
  final(self)
  for key in pairs(self) do
    self[key] = nil
  end
end

function Actor:body(expression, facing)
  if(expression == nil) then
    expression = ""
  end
  if(facing == nil) then
    facing = 0
  end
  MetallicCrow.actor_body(self.data, expression, facing)
end

function Actor:eyes(expression)
  MetallicCrow.actor_eyes(self.data, expression)
end

function Actor:mouth(expression)
  MetallicCrow.actor_mouth(self.data, expression)
end

function Actor:view()
  MetallicCrow.view(self.data)
end

function Actor:view_also()
  MetallicCrow.view_add(self.data)
end

function Actor:position(x, y)
  MetallicCrow.actor_position(self.data, x, y)
end

function Actor:velocity(u, u)
  MetallicCrow.actor_velocity(self.data, u, v)
end

function Actor:force(f, g)
  MetallicCrow.actor_force(self.data, f, g)
end

function Actor:impulse(i, j)
  MetallicCrow.actor_impulse(self.data, i, j)
end

function Actor:modulation(r, g, b, a)
  MetallicCrow.actor_modulation(self.data, r, g, b, a)
end

function Actor:rotation(angle)
  MetallicCrow.actor_rotation(self.data, angle)
end

function Actor:dilation(dilation)
  MetallicCrow.actor_dilation(self.data, dilation)
end

function Actor:scale(scale)
  MetallicCrow.actor_scale(self.data, scale)
end

function Actor:plane(plane)
  MetallicCrow.actor_plane(self.data, plane)
end

function Actor:link(group)
  MetallicCrow.actor_link(self.data, group)
end

function Actor:unlink(group)
  MetallicCrow.actor_unlink(self.data, group)
end

function Actor.new(stage, config)
  local self = {}
  setmetatable(self, Actor)
  self.data = MetallicCrow.actor_load(stage.data, Load.load(config))
  return self
end

function Actor.view_list(actors)
  local key, actor = next(actors, nil)
  actor:view()
  for key, actor in pairs(actors) do
    actor:view_also()
  end
end

return Actor