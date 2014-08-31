#include "animation.h"
#include "json_iterator.h"
namespace display
{
Animation MakeAnimation(json::JSON const& json, display::Window& window)
{
  char const* page;
  double width;
  double height;
  json_t* frames;

  json.Unpack("{sssfsfso}", 
    "page", &page,
    "width", &width,
    "height", &height,
    "frames", &frames);

  display::Texture texture(page, window);
  Animation animation;

  for(json::JSON const& value : json::JSON(frames))
  {
    double x;
    double y;
    value.Unpack("[ff]", &x, &y);
    animation.emplace_back(texture, display::BoundingBox((float)x, (float)y, (float)width, (float)height));
  }

  return animation;
}
}