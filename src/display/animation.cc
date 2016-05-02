#include "animation.h"
namespace display
{
auto MakeAnimation(lua::Stack& lua, Window& window, boost::filesystem::path const& path) -> Animation
{
  Animation animation;
  for(int index = 1, end = lua.Size(); index <= end; ++index)
  {
    lua::Guard guard = lua.Field(index);
    
    std::string page = lua.Field<std::string>("page");

    {
      lua::Guard guard = lua.Field("clip");
      animation.emplace_back(path / page, window, BoundingBox(lua));
    }
  }
  return animation;
}
}