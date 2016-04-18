local MetallicCrow = require "metallic_crow"
local Load = require "mcl/load"

local Stage = {}

local function final(self)
  if self.data then
    MetallicCrow.stage_free(self.data)
  end
end

Stage.__gc = final
Stage.__index = Stage

function Stage:delete()
  final(self)
  for key in pairs(self) do
      self[key] = nil
  end
end

function Stage:show()
  MetallicCrow.stage_nominate(self.data)
end

function Stage:ambient(r, g, b)
  MetallicCrow.stage_ambient(self.data, r, g, b)
end

function Stage:pause()
  MetallicCrow.stage_pause(self.data)
end

function Stage:resume()
  MetallicCrow.stage_resume(self.data)
end

function Stage:collision_begin(group_a, group_b, command)
  MetallicCrow.collision_begin(self.data, group_a, group_b, command)
end

function Stage:collision_end(group_a, group_b, command)
  MetallicCrow.collision_end(self.data, group_a, group_b, command)  
end

function Stage:collision_link(group_a, group_b)
  MetallicCrow.collision_link(self.data, group_a, group_b)
end

function Stage:collision_unlink(group_a, group_b)
  MetallicCrow.collision_unlink(self.data, group_a, group_b)
end

function Stage:choice(up, down, left, right, interval)
  MetallicCrow.choice(self.data, up, down, left, right, interval)
end

function Stage:choice_modulation(choice, r, g, b, a)
  if(choice == "up") then
    MetallicCrow.choice_up_modulation(self.data, r, g, b, a)
  elseif(choice == "down") then
    MetallicCrow.choice_down_modulation(self.data, r, g, b, a)
  elseif(choice == "left") then
    MetallicCrow.choice_left_modulation(self.data, r, g, b, a)
  elseif(choice == "right") then
    MetallicCrow.choice_right_modulation(self.data, r, g, b, a)
  end
end

function Stage:text(text)
  MetallicCrow.subtitle_text(self.data, text)
end

function Stage:text_modulation(r, g, b, a)
  MetallicCrow.subtitle_modulation(self.data, r, g, b, a)
end

function Stage:view(z)
  MetallicCrow.view_zoom(self.data, z)
end

function Stage:rotation(angle)
  MetallicCrow.view_rotation(self.data, angle)
end

function Stage.new(config)
  local self = {}
  setmetatable(self, Stage)
  self.data = MetallicCrow.stage_load(Load.load(config))
  return self
end

return Stage