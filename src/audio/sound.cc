#include "sound.h"
#include <unordered_map>
#include "SDL_mixer.h"
#include "chunk.h"
#include <mutex>
namespace audio
{
class SoundImpl
{
public:
  SoundImpl(std::string const& filename);
  void Play(int repeats);
  void Pause(void);
  void Resume(void);
  void Stop(void);
  void Fade(int ms);
  void Volume(int volume);

  mix::Chunk chunk_;
  int volume_;
};

static std::mutex mutex;
static std::unordered_map<int, SoundImpl const*> channels;

static void ChannelFinished(int channel)
{
  std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
  if(-1 == std::try_lock(lock))
  {
    (void)channels.erase(channel);
  }
}

static void InitChannelMixer(void)
{
  static bool initialised;
  if(!initialised)
  {
    Mix_ChannelFinished(ChannelFinished);
    initialised = true;
  }
}

static int const default_volume = -1;
SoundImpl::SoundImpl(std::string const& filename) : chunk_(filename), volume_(default_volume)
{
  InitChannelMixer();
}

void SoundImpl::Play(int repeats)
{
  int channel = chunk_.Play(repeats, volume_);
  if(channel != -1)
  {
    channels[channel] = this;
  }
}

void SoundImpl::Pause(void)
{
  for(auto& channel : channels)
  {
    if(channel.second == this)
    {
      Mix_Pause(channel.first);
    }
  }
}

void SoundImpl::Resume(void)
{
  for(auto& channel : channels)
  {
    if(channel.second == this)
    {
      Mix_Resume(channel.first);
    }
  }
}

void SoundImpl::Stop(void)
{
  for(auto channel = channels.begin(); channel != channels.end();)
  {
    if(channel->second == this)
    {
      (void)Mix_HaltChannel(channel->first);
      channel = channels.erase(channel);
    }
    else
    {
      ++channel;
    }
  }
}

void SoundImpl::Fade(int ms)
{
  for(auto channel = channels.begin(); channel != channels.end();)
  {
    if(channel->second == this)
    {
      (void)Mix_FadeOutChannel(channel->first, ms);
      channel = channels.erase(channel);
    }
    else
    {
      ++channel;
    }
  }
}

void SoundImpl::Volume(int volume)
{
  volume_ = volume;
  for(auto& channel : channels)
  {
    if(channel.second == this)
    {
      (void)Mix_Volume(channel.first, volume_);
    }
  }
}

Sound::Sound(std::string const& filename)
{
  impl_ = std::make_shared<SoundImpl>(filename);
}

void Sound::Play(int repeats)
{
  std::lock_guard<std::mutex> lock(mutex);
  impl_->Play(repeats);
}

void Sound::Pause(void) const
{
  std::lock_guard<std::mutex> lock(mutex);
  impl_->Pause();
}

void Sound::Resume(void) const
{
  std::lock_guard<std::mutex> lock(mutex);
  impl_->Resume();
}

void Sound::Stop(void) const
{
  std::lock_guard<std::mutex> lock(mutex);
  impl_->Stop();
}

void Sound::Fade(int ms) const
{
  std::lock_guard<std::mutex> lock(mutex);
  impl_->Fade(ms);
}

void Sound::Volume(int volume)
{
  std::lock_guard<std::mutex> lock(mutex);
  impl_->Volume(volume);
}

Sound::operator bool(void) const
{
  return bool(impl_);
}
}
