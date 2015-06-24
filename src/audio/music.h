#ifndef MUSIC_H_
#define MUSIC_H_
#include <memory>
#include "boost/filesystem.hpp"
#include "lua_stack.h"
namespace audio
{
class Music
{
public:
  Music() = default; 
  Music(lua::Stack& lua, boost::filesystem::path const& path);
  Music(boost::filesystem::path const& file, float volume, bool repeat);
  auto Pause() -> void;
  auto Resume() -> void;
  auto operator()(float volume) -> bool;
  auto End() -> void;
  explicit operator bool() const;
  static auto Free(boost::filesystem::path const& file) -> void;
  static auto Load(boost::filesystem::path const& file) -> void;
  class Impl;
private:
  std::shared_ptr<Impl> impl_;
};
}
#endif