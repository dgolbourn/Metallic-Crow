#ifndef ANIMATION_H_
#define ANIMATION_H_
#include "window.h"
#include "json.h"
#include <vector>
#include "boost/filesystem.hpp"
namespace display
{
typedef std::vector<Texture> Animation;
Animation MakeAnimation(json::JSON&& json, Window& window, boost::filesystem::path const& path);
}
#endif