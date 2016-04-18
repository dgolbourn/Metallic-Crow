local Timer = require "mcl/timer"
local Sound = require "mcl/sound"
local Actor = require "mcl/actor"

local Speak = {}

local speaks = {}
setmetatable(speaks, {__mode = "k"})

function Speak.speak(stage, actor, text, colour, sound, focus, zoom, text_interval, expressions)
  if(speaks[stage] == nil) then
    speaks[stage] = {}
  end

  if speaks[stage].text_timer then
    speaks[stage].text_timer:delete()
    speaks[stage].text_timer = nil
    speaks[stage].text_clear = nil
  end
  if text_interval then
    speaks[stage].text_clear = function() stage:text(" ") end
    speaks[stage].text_timer = Timer.new(stage, speaks[stage].text_clear, text_interval, 0)
  end

  if text then
    stage:text(text)
  else
    stage:text(" ")
  end
  if colour then
    stage:text_modulation(colour.r, colour.g, colour.b, colour.a)
  end

  if focus then
    Actor.view_list(focus)
  end
  if zoom then
    stage:view(zoom)
  end

  if(speaks[actor] == nil) then
    speaks[actor] = {}
  end

  if speaks[actor].sound then
    speaks[actor].sound:delete()
    speaks[actor].sound = nil
  end
  if sound then
    speaks[actor].sound = Sound.new(stage, sound)
    speaks[actor].sound:play()
  end

  if speaks[actor].expressions then
    for key, expression in pairs(speaks[actor].expressions) do
      expression.timer:delete()
    end
    speaks[actor].expressions = nil
  end
  if expressions then
    local index = 1
    speaks[actor].expressions = {}
    for key, pair in pairs(expressions) do
      if (pair[1] == 0) then
        actor:mouth(pair[2])
      else
        local expression = {}
        expression.command = function() actor:mouth(pair[2]) end
        expression.timer = Timer.new(stage, expression.command, pair[1], 0)
        speaks[actor].expressions[index] = expression
        index = index + 1
      end
    end
  end
end

return Speak