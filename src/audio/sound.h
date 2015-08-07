#ifndef SOUND_H_
#define SOUND_H_
#include <memory>
#include "boost/filesystem.hpp"
#include "lua_stack.h"
#include "weak_ptr.h"
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
  class Impl;
private:
  std::shared_ptr<Impl> impl_;
public:
  bool operator==(Sound const& other) const;
  typedef memory::WeakPtr<Sound> WeakPtr;
  friend WeakPtr;
  static size_t Hash(Sound const& sound);
};
}

namespace std 
{
template<> struct hash<audio::Sound> 
{
  size_t operator()(audio::Sound const& sound) const 
  { 
    return audio::Sound::Hash(sound); 
  }
};
}
#endif