#ifndef SOUND_H_
#define SOUND_H_
#include <memory>
#include "boost/filesystem.hpp"
namespace audio
{
class Sound
{
public:
  Sound() = default;
  Sound(boost::filesystem::path const& file, float volume, bool repeat);
  void Pause();
  void Resume();
  bool operator()();
  explicit operator bool() const;
  class Impl;
private:
  std::shared_ptr<Impl> impl_;
};

void Free();
void Free(boost::filesystem::path const& file);
}
#endif