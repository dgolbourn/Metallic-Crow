#include "animation.h"
#include "json_iterator.h"
namespace display
{
Animation MakeAnimation(json::JSON&& json, display::Window& window, boost::filesystem::path const& path)
{
  Animation animation;
  for(json::JSON const& value : json)
  {
    char const* page;
    json_t* clip;
    value.Unpack("{ssso}", 
      "page", &page, 
      "clip", &clip);
    animation.emplace_back(display::Texture(path / page, window) , display::BoundingBox(json::JSON(clip)));
  }
  return animation;
}
}