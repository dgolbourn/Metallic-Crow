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
#include "mix_thread.h"
#include <mutex>
namespace
{
typedef std::shared_ptr<Mix_Chunk> Chunk;
typedef std::weak_ptr<Mix_Chunk> WeakChunk;
}

namespace audio
{
class Sound::Impl
{
public:
  Impl(boost::filesystem::path const& file, float volume, bool repeat);
  ~Impl();
  auto Pause() -> void;
  auto Resume() -> void;
  auto End() -> void;
  auto Play(float volume) -> void;
  mix::Library mix_;
  Chunk chunk_;
  int channel_;
  bool repeat_;
  float play_volume_;
  float volume_;
  bool paused_;
  bool end_;
};
}

namespace
{
auto Init(boost::filesystem::path const& file) -> Mix_Chunk*
{
  Mix_Chunk* chunk = Mix_LoadWAV(file.string().c_str());
  if(!chunk)
  {
    BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
  }
  return chunk;
}

std::unordered_map<boost::filesystem::path, WeakChunk, boost::hash<boost::filesystem::path>> chunks;

auto MakeChunk(boost::filesystem::path const& file) -> Chunk
{
  Chunk chunk;
  auto fileiter = chunks.find(file);
  if(fileiter != chunks.end())
  {
    chunk = fileiter->second.lock();
  }

  if(!chunk)
  {
    chunk = Chunk(Init(file), Mix_FreeChunk);
    chunks.emplace(file, chunk);
  }
  return chunk;
}

std::unordered_map<boost::filesystem::path, Chunk, boost::hash<boost::filesystem::path>> cache;

std::vector<audio::Sound::Impl*> sounds;

auto ChannelFinished(int channel) noexcept -> void
{
  try
  {
    std::lock_guard<mix::Mutex> lock(mix::mutex);
    if(channel < static_cast<int>(sounds.size()))
    {
      audio::Sound::Impl* sound = sounds[channel];
      sounds[channel] = nullptr;
      if(sound)
      {
        sound->channel_ = -1;
        if(sound->repeat_ && !sound->end_)
        {
          sound->Play(sound->play_volume_);
        }
      }
    }
  }
  catch(...)
  {
    exception::Log("Swallowed exception");
  }
}

auto InitHookSound() -> void
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
auto Sound::Load(boost::filesystem::path const& file) -> void
{
  cache.emplace(file, MakeChunk(file));
}

auto Sound::Free(boost::filesystem::path const& file) -> void
{
  cache.erase(file);
}

auto Sound::Impl::Play(float volume) -> void
{
  std::lock_guard<mix::Mutex> lock(mix::mutex);
  play_volume_ = volume;
  if(channel_ == -1)
  {
    int size = Mix_AllocateChannels(-1);
    if(Mix_Playing(-1) < size)
    {
      channel_ = Mix_PlayChannel(-1, chunk_.get(), 0);
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
    Mix_Volume(channel_, static_cast<int>(play_volume_ * volume_ * MIX_MAX_VOLUME));
  }
  end_ = false;
}

Sound::Impl::Impl(boost::filesystem::path const& file, float volume, bool repeat) : channel_(-1), repeat_(repeat), play_volume_(1.f), volume_(volume), paused_(true), end_(false)
{
  InitHookSound();

  chunk_ = MakeChunk(file);
}

Sound::Impl::~Impl()
{
  std::lock_guard<mix::Mutex> lock(mix::mutex);
  if(channel_ != -1)
  {
    end_ = true;
    Mix_HaltChannel(channel_);
  }
}

auto Sound::Impl::Pause() -> void
{
  std::lock_guard<mix::Mutex> lock(mix::mutex);
  paused_ = true;
  if(channel_ != -1)
  {
    Mix_Pause(channel_);
  }
}

auto Sound::Impl::Resume() -> void
{
  std::lock_guard<mix::Mutex> lock(mix::mutex);
  paused_ = false;
  if(channel_ != -1)
  {
    Mix_Resume(channel_);
  }
}

auto Sound::Impl::End() -> void
{
  std::lock_guard<mix::Mutex> lock(mix::mutex);
  end_ = true;
}

Sound::Sound(boost::filesystem::path const& file, float volume, bool repeat) : impl_(std::make_shared<Impl>(file, volume, repeat))
{
}

Sound::Sound(lua::Stack& lua, boost::filesystem::path const& path)
{
  float volume;
  {
    lua::Guard guard = lua.Field("volume");
    lua.Pop(volume);
  }

  std::string file;
  { 
    lua::Guard guard = lua.Field("file");
    lua.Pop(file);
  }

  bool repeat;
  {
    lua::Guard guard = lua.Field("repeat");
    lua.Pop(repeat);
  }

  impl_ = std::make_shared<Impl>(path / file, volume, repeat);
}

auto Sound::Pause() -> void
{
  impl_->Pause();
}

auto Sound::Resume() -> void
{
  impl_->Resume();
}

auto Sound::End() -> void
{
  impl_->End();
}

auto Sound::operator()(float volume) -> bool
{
  bool valid = bool(impl_);
  if(valid)
  {
    impl_->Play(volume);
  }
  return valid;
}

Sound::operator bool() const
{
  bool valid = bool(impl_);
  if(valid)
  {
    std::lock_guard<mix::Mutex> lock(mix::mutex);
    valid = (impl_->channel_ != -1);
  }
  return valid;
}

size_t Sound::Hash(Sound const& sound)
{
  return std::hash<std::shared_ptr<Impl>>()(sound.impl_);
}

bool Sound::operator==(Sound const& other) const
{
  return impl_ == other.impl_;
}
}
