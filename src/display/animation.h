#ifndef ANIMATION_H_
#define ANIMATION_H_
#include "window.h"
#include "lua_stack.h"
#include <vector>
#include "boost/filesystem.hpp"
#include "timeslice.h"
namespace display
{
typedef std::vector<Texture> Animation;
auto MakeAnimation(lua::Stack& lua, Window& window, boost::filesystem::path const& path, event::Timeslice& loader) -> Animation;
}
#endif