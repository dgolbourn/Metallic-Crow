#ifndef MUSIC_H_
#define MUSIC_H_
#include <memory>
#include "boost/filesystem.hpp"
#include "json.h"
namespace audio
{
class Music
{
public:
  Music() = default; 
  Music(json::JSON const& json, boost::filesystem::path const& path);
  Music(boost::filesystem::path const& file, float volume, bool repeat);
  void Pause();
  void Resume();
  bool operator()(float volume);
  void End();
  explicit operator bool() const;
  static void Free(boost::filesystem::path const& file);
  static void Load(boost::filesystem::path const& file);
  class Impl;
private:
  std::shared_ptr<Impl> impl_;
};
}
#endif