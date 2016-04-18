local Package = require "package"

local Load = {}

function Load.load(file)
  if type(file) == "string" then
    return dofile(Package.searchpath(file, Package.path))
  end
  return file
end

return Load