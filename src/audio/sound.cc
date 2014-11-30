#include "sound.h"
#include <unordered_map>
#include "SDL_mixer.h"
#include "mix_exception.h"
#include "mix_library.h"
#include <mutex>
#include "log.h"
#include <iostream>
#include <vector>
#include "boost/functional/hash.hpp"
#include "audio_thread.h"
namespace
{
typedef std::shared_ptr<Mix_Chunk> Chunk;
}

namespace audio
{
class Sound::Impl
{
public:
  Impl(boost::filesystem::path const& file, float volume, bool repeat);
  ~Impl();
  void Pause();
  void Resume();
  void Play(float volume);
  mix::Library mix_;
  Chunk chunk_;
  int channel_;
  int loops_;
  float volume_;
  bool paused_;
};
}

namespace
{
Mix_Chunk* Init(boost::filesystem::path const& file)
{
  Mix_Chunk* chunk = Mix_LoadWAV(file.string().c_str());
  if(!chunk)
  {
    BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
  }
  return chunk;
}

std::unordered_map<boost::filesystem::path, Chunk, boost::hash<boost::filesystem::path>> chunks;
std::vector<audio::Sound::Impl*> sounds;

void ChannelFinished(int channel) noexcept
{
  try
  {
    audio::Guard lock(audio::mutex);
    if(channel < int(sounds.size()))
    {
      if(sounds[channel])
      {
        sounds[channel]->channel_ = -1;
        sounds[channel] = nullptr;
      }
    }
  }
  catch(...)
  {
    exception::Log("Swallowed exception");
  }
}

void InitHookSound()
{
  static bool initialised;
  if(!initialised)
  {
    Mix_ChannelFinished(ChannelFinished);
    initialised = true;
  }
}
}

namespace audio
{
void Sound::Free()
{
  chunks.clear();
}

void Sound::Free(boost::filesystem::path const& file)
{
  chunks.erase(file);
}

void Sound::Impl::Play(float volume)
{
  Guard lock(mutex);
  if(channel_ == -1)
  {
    int size = Mix_AllocateChannels(-1);
    if(Mix_Playing(-1) < size)
    {
      channel_ = Mix_PlayChannel(-1, chunk_.get(), loops_);
      if(channel_ == -1)
      {
        BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
      }
      if(paused_)
      {
        Mix_Pause(channel_);
      }
      sounds.resize(size);
      sounds[channel_] = this;
    }
  }
  if(channel_ != -1)
  {
    Mix_Volume(channel_, int(volume * volume_ * MIX_MAX_VOLUME));
  }
}

Sound::Impl::Impl(boost::filesystem::path const& file, float volume, bool repeat) : channel_(-1), loops_(repeat ? -1 : 0), volume_(volume), paused_(true)
{
  InitHookSound();

  auto fileiter = chunks.find(file);
  if(fileiter != chunks.end())
  {
    chunk_ = fileiter->second;
  }
  else
  {
    chunk_ = Chunk(Init(file), Mix_FreeChunk);
    chunks.emplace(file, chunk_);
  }
}

Sound::Impl::~Impl()
{
  Guard lock(mutex);
  if(channel_ != -1)
  {
    Mix_HaltChannel(channel_);
  }
}

void Sound::Impl::Pause()
{
  Guard lock(mutex);
  paused_ = true;
  if(channel_ != -1)
  {
    Mix_Pause(channel_);
  }
}

void Sound::Impl::Resume()
{
  Guard lock(mutex);
  paused_ = false;
  if(channel_ != -1)
  {
    Mix_Resume(channel_);
  }
}

Sound::Sound(boost::filesystem::path const& file, float volume, bool repeat) : impl_(std::make_shared<Impl>(file, volume, repeat))
{
}

Sound::Sound(json::JSON const& json, boost::filesystem::path const& path)
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

void Sound::Pause()
{
  impl_->Pause();
}

void Sound::Resume()
{
  impl_->Resume();
}

bool Sound::operator()(float volume)
{
  bool valid = false;
  if(impl_)
  {
    impl_->Play(volume);
    valid = true;
  }
  return valid;
}

Sound::operator bool() const
{
  bool valid = bool(impl_);
  if(valid)
  {
    Guard lock(mutex);
    valid = (impl_->channel_ != -1);
  }
  return valid;
}
}
