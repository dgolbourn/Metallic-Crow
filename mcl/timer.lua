local MetallicCrow = require "metallic_crow"

local Timer = {}

local function final(self)
  if self.data then
    MetallicCrow.timer_free(self.data)
  end
end

Timer.__gc = final
Timer.__index = Timer

function Timer:delete()
  final(self)
  for key in pairs(self) do
      self[key] = nil
  end
end

function Timer.new(stage, command, interval, loops)
  local self = {}
  setmetatable(self, Timer)
  self.data = MetallicCrow.timer_load(stage.data, command, interval, loops)
  return self
end

local timers = {}
setmetatable(timers, {__mode = "k"})

function Timer.timer(stage, command, interval, loops)
  if timers[stage] then
    timers[stage]:delete()
    timers[stage] = nil
  end
  if command then
    if(loops == nil) then
      loops = 0
    end
    timers[stage] = Timer.new(stage, command, interval, loops)
  end
end

return Timer