#ifndef ANIMATION_H_
#define ANIMATION_H_
#include "window.h"
#include "lua_stack.h"
#include <vector>
#include "boost/filesystem.hpp"
namespace display
{
typedef std::vector<Texture> Animation;
Animation MakeAnimation(lua::Stack& lua, Window& window, boost::filesystem::path const& path);
}
#endif