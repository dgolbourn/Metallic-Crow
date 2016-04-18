local MetallicCrow = require "metallic_crow"
local Load = require "mcl/load"

local Music = {}

local function final(self)
  if self.data then
    MetallicCrow.music_free(self.data)
  end
end

Music.__gc = final
Music.__index = Music

function Music:delete()
  final(self)
  for key in pairs(self) do
      self[key] = nil
  end
end

function Music:play()
  MetallicCrow.music_play(self.data)
end

function Music:stop()
  MetallicCrow.music_end(self.data)
end

function Music.new(stage, config)
  local self = {}
  setmetatable(self, Music)
  self.data = MetallicCrow.music_load(stage.data, Load.load(config))
  return self
end

return Music