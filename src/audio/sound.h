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
  auto Pause() -> void;
  auto Resume() -> void;
  auto End() -> void;
  auto operator()(float volume) -> bool;
  explicit operator bool() const;
  static auto Free(boost::filesystem::path const& file) -> void;
  static auto Load(boost::filesystem::path const& file) -> void;
  class Impl;
private:
  std::shared_ptr<Impl> impl_;
};
}
#endif