local MetallicCrow = require "metallic_crow"
local Load = require "mcl/load"

local Sound = {}

local function final(self)
  if self.data then
    MetallicCrow.sound_free(self.data)
  end
end

Sound.__gc = final
Sound.__index = Sound

function Sound:delete()
  final(self)
  for key in pairs(self) do
      self[key] = nil
  end
end

function Sound:play()
  MetallicCrow.sound_play(self.data)
end

function Sound:stop()
  MetallicCrow.sound_end(self.data)
end

function Sound.new(stage, config)
  local self = {}
  setmetatable(self, Sound)
  self.data = MetallicCrow.sound_load(stage.data, Load.load(config))
  return self
end

return Sound