#include <map>
#include "music.h"
#include "SDL_mixer.h"
#include "mix_library.h"
#include "mix_exception.h"
#include <unordered_map>
#include "boost/functional/hash.hpp"
namespace
{
typedef std::shared_ptr<Mix_Music> Stream;
}

namespace audio
{
class Music::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(boost::filesystem::path const& file, float volume, bool repeat);
  void Play(float volume);
  void Pause();
  void Resume();
  mix::Library mix_;
  Stream stream_;
  float volume_;
  int loops_;
  bool complete_;
  bool paused_;
};
}

namespace
{
std::weak_ptr<audio::Music::Impl> music;

void MusicFinished()
{
  if(auto current = music.lock())
  {
    current->complete_ = true;
  }
}

void InitHookMusic()
{
  static bool initialised;
  if(!initialised)
  {
    initialised = true;
    Mix_HookMusicFinished(MusicFinished);
  }
}

Mix_Music* Init(boost::filesystem::path const& file)
{
  Mix_Music* music = Mix_LoadMUS(file.string().c_str());
  if(!music)
  {
    BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
  }
  return music;
}

std::unordered_map<boost::filesystem::path, Stream, boost::hash<boost::filesystem::path>> streams;
}

namespace audio
{
void Music::Free()
{
  streams.clear();
}

void Music::Free(boost::filesystem::path const& file)
{
  streams.erase(file.string());
}

void Music::Impl::Pause()
{
  paused_ = true;
  if(music.lock() == shared_from_this())
  {
    Mix_PauseMusic();
  }
}

void Music::Impl::Resume()
{
  paused_ = false;
  if(music.lock() == shared_from_this())
  {
    Mix_ResumeMusic();
  }
}

void Music::Impl::Play(float volume)
{
  if(music.lock() != shared_from_this())
  {
    complete_ = false;
    if(Mix_PlayMusic(stream_.get(), loops_) == -1)
    {
      BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
    }
    if(paused_)
    {
      Mix_PauseMusic();
    }
    music = shared_from_this();
  }
  Mix_VolumeMusic(int(volume_ * volume / MIX_MAX_VOLUME));
}

Music::Impl::Impl(boost::filesystem::path const& file, float volume, bool repeat) : loops_(repeat ? -1 : 0), volume_(volume), complete_(false), paused_(true)
{
  InitHookMusic();

  auto fileiter = streams.find(file);
  if(fileiter != streams.end())
  {
    stream_ = fileiter->second;
  }
  else
  {
    stream_ = Stream(Init(file), Mix_FreeMusic);
    streams.emplace(file, stream_);
  }
}

Music::Music(boost::filesystem::path const& file, float volume, bool repeat) : impl_(std::make_shared<Impl>(file, volume, repeat))
{
}

Music::Music(json::JSON const& json, boost::filesystem::path const& path)
{
  char const* file;
  double volume;
  int repeat;
  json.Unpack("{sssfsb}",
    "file", &file,
    "volume", &volume,
    "repeat", &repeat);
  impl_ = std::make_shared<Impl>(path / file, float(volume), repeat != 0);
}

bool Music::operator()(float volume)
{
  bool valid = false;
  if(impl_)
  {
    impl_->Play(volume);
    valid = true;
  }
  else
  {
    Mix_HaltMusic();
  }
  return valid;
}

Music::operator bool() const
{
  return bool(impl_) && !impl_->complete_;
}

void Music::Pause()
{
  impl_->Pause();
}

void Music::Resume()
{
  impl_->Resume();
}
}