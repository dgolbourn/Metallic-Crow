#ifndef MUSIC_H_
#define MUSIC_H_
#include <memory>
#include "boost/filesystem.hpp"
namespace audio
{
class Music
{
public:
  Music() = default;
  Music(boost::filesystem::path const& file, float volume, bool repeat);
  void Pause();
  void Resume();
  bool operator()();
  explicit operator bool() const;
  class Impl;
private:
  std::shared_ptr<Impl> impl_;
};
}
#endif