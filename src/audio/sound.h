#ifndef SOUND_H_
#define SOUND_H_
#include <memory>
#include "boost/filesystem.hpp"
#include "lua_stack.h"
namespace audio
{
class Sound
{
public:
  Sound() = default;
  Sound(lua::Stack& lua, boost::filesystem::path const& path);
  Sound(boost::filesystem::path const& file, float volume, bool repeat);
  void Pause();
  void Resume();
  void End();
  bool operator()(float volume);
  explicit operator bool() const;
  static void Free(boost::filesystem::path const& file);
  static void Load(boost::filesystem::path const& file);
  class Impl;
private:
  std::shared_ptr<Impl> impl_;
};
}
#endif