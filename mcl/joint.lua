local MetallicCrow = require "metallic_crow"
local Load = require "mcl/load"

local Joint = {}

local function final(self)
  if self.data then
    MetallicCrow.joint_free(self.data)
  end
end

Joint.__gc = final
Joint.__index = Joint

function Joint:delete()
  final(self)
  for key in pairs(self) do
    self[key] = nil
  end
end

function Joint.new(config, actor_a, actor_b)
  local self = {}
  setmetatable(self, Joint)
  self.data = MetallicCrow.joint_load(Load.load(config), actor_a.data, actor_b.data)
  return self
end

return Joint