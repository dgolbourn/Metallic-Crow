#include "animation.h"
#include "json_iterator.h"
namespace display
{
Animation MakeAnimation(json::JSON const& json, display::Window& window)
{
  Animation animation;
  for(json::JSON const& value : json::JSON(json))
  {
    char const* page;
    json_t* clip;
    value.Unpack("{ssso}", 
      "page", &page, 
      "clip", &clip);
    animation.emplace_back(display::Texture(page, window) , display::BoundingBox(json::JSON(clip)));
  }
  return animation;
}
}