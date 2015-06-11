#include "version.h"
#include "boost/config.hpp"
#include "boost/version.hpp"
#include "lua.h"
#include "sdl.h"
#include "sdl_mixer.h"
#include "sdl_image.h"
#include "sdl_ttf.h"
#include "Box2D/Box2D.h"
#include <sstream>
namespace config
{
std::string Version()
{
  static const int major = 0;
  static const int minor = 2;
  static const int patch = 0;
  static const std::string pre("");
  static const std::string build("");

  std::stringstream stream;
  stream << "Version: ";
  stream << major << "." << minor << "." << patch;
  if(pre != "")
  {
    stream << "-" << pre;
  }
  if(build != "")
  {
    stream << "+" << build;
  }
  stream << std::endl;

  stream << "Platform: " << BOOST_PLATFORM << std::endl;
  stream << "Compiler: " << BOOST_COMPILER << std::endl;
  stream << "Standard Library: " << BOOST_STDLIB << std::endl;
  stream << "Boost: " << BOOST_VERSION / 100000 << "." << BOOST_VERSION / 100 % 1000 << "." <<  BOOST_VERSION % 100  << std::endl;
  stream << "Lua: " << LUA_VERSION_MAJOR << "." << LUA_VERSION_MINOR << "." << LUA_VERSION_RELEASE << std::endl;
  stream << "SDL: " << SDL_MAJOR_VERSION << "." << SDL_MINOR_VERSION << "." << SDL_PATCHLEVEL << std::endl;
  stream << "SDL Mixer: " << SDL_MIXER_MAJOR_VERSION << "." << SDL_MIXER_MINOR_VERSION << "." << SDL_MIXER_PATCHLEVEL << std::endl;
  stream << "SDL Image: " << SDL_IMAGE_MAJOR_VERSION << "." << SDL_IMAGE_MINOR_VERSION << "." << SDL_IMAGE_PATCHLEVEL << std::endl;
  stream << "SDL TTF: " << SDL_TTF_MAJOR_VERSION << "." << SDL_TTF_MINOR_VERSION << "." << SDL_TTF_PATCHLEVEL << std::endl;
  stream << "Box2D: " << b2_version.major << "." << b2_version.minor << "." << b2_version.revision << std::endl;
  return stream.str();
}
}