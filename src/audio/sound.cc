#include "sound.h"
#include "sound_impl.h"
#include "mix_exception.h"
#include <unordered_map>
#include "chunk.h"

namespace audio
{
typedef std::weak_ptr<class SoundImpl> SoundPtr;

class SoundImpl
{
public:
  SoundImpl(std::string const& filename);
  void Play(int loops, SoundPtr const& impl);
  void Pause(void) const;
  void Resume(void) const;
  void Stop(void) const;
  void Volume(int volume);

  mix::Chunk chunk_;
  int volume_;
  int channel_;
};

static bool initialised;
static int const no_channel = -1;
static std::unordered_map<int, SoundPtr> active_channels;

static void ChannelFinishedCallback(int channel)
{
  auto sound_iter = active_channels.find(channel);
  if(sound_iter != active_channels.end())
  {
    if(auto sound = sound_iter->second.lock())
    {
      sound->channel_ = no_channel;
      active_channels.erase(sound_iter);
    }
  }
}

static void Init(void)
{
  if(!initialised)
  {
    Mix_ChannelFinished(ChannelFinishedCallback);
    initialised = true;
  }
}

static int const default_volume = -1;
SoundImpl::SoundImpl(std::string const& filename) : chunk_(filename), volume_(default_volume), channel_(no_channel)
{
  Init();
}

void SoundImpl::Play(int loops, SoundPtr const& impl)
{
  if(channel_ == no_channel)
  {
    channel_ = chunk_.Play(loops, volume_);
    active_channels[channel_] = impl;
  }
}

void SoundImpl::Pause(void) const
{
  if(channel_ != no_channel)
  {
    Mix_Pause(channel_);
  }
}

void SoundImpl::Resume(void) const
{
  if(channel_ != no_channel)
  {
    Mix_Resume(channel_);
  }
}

void SoundImpl::Stop(void) const
{
  if(channel_ != no_channel)
  {
    Mix_HaltChannel(channel_);
  }
}

void SoundImpl::Volume(int volume)
{
  volume_ = volume;
}

Sound::Sound(std::string const& filename)
{
  impl_ = std::shared_ptr<SoundImpl>(new SoundImpl(filename));
}

Sound::Sound(void)
{
}

Sound::Sound(Sound const& other) : impl_(other.impl_)
{
}

Sound::Sound(Sound&& other) : impl_(std::move(other.impl_))
{
}

Sound::~Sound(void)
{
}

Sound& Sound::operator=(Sound other)
{
  std::swap(impl_, other.impl_);
  return *this;
}

void Sound::Play(int loops)
{
  impl_->Play(loops, impl_);
}

void Sound::Pause(void) const
{
  impl_->Pause();
}

void Sound::Resume(void) const
{
  impl_->Resume();
}

void Sound::Stop(void) const
{
  impl_->Stop();
}

void Sound::Volume(int volume)
{
  impl_->Volume(volume);
}
}
