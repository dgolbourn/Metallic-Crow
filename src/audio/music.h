#ifndef MUSIC_H_
#define MUSIC_H_
#include <memory>
#include "boost/filesystem.hpp"
#include "lua_stack.h"
#include "weak_ptr.h"
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
  class Impl;
private:
  std::shared_ptr<Impl> impl_;
public:
  bool operator==(Music const& other) const;
  typedef memory::WeakPtr<Music> WeakPtr;
  friend WeakPtr;
  static size_t Hash(Music const& music);
};
}

namespace std 
{
template<> struct hash<audio::Music> 
{
  size_t operator()(audio::Music const& music) const 
  { 
    return audio::Music::Hash(music); 
  }
};
}
#endif