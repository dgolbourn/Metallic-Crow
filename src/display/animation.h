#ifndef ANIMATION_H_
#define ANIMATION_H_
#include "window.h"
#include "json.h"
#include <vector>
namespace display
{
typedef std::vector<Texture> Animation;
Animation MakeAnimation(json::JSON const& json, Window& window);
}
#endif