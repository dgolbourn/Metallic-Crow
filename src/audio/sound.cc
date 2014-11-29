#include "sound.h"
#include <unordered_map>
#include "SDL_mixer.h"
#include "mix_exception.h"
#include "mix_library.h"
#include <mutex>
#include "log.h"
#include <iostream>
namespace
{
typedef std::shared_ptr<Mix_Chunk> Chunk;
}

namespace audio
{
class Sound::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(boost::filesystem::path const& file, float volume, bool repeat);
  ~Impl();
  void Pause();
  void Resume();
  void Play();
  mix::Library mix_;
  Chunk chunk_;
  int channel_;
  int loops_;
  int volume_;
  bool play_on_resume_;
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

std::unordered_map<std::string, Chunk> chunks;
std::mutex mutex;
std::unordered_map<int, std::shared_ptr<audio::Sound::Impl>> channels;

void ChannelFinished(int channel) noexcept
{
  try
  {
    std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
    if(-1 == std::try_lock(lock))
    {
      auto iter = channels.find(channel);
      if(iter != channels.end())
      {
        iter->second->channel_ = -1;
        channels.erase(iter);
      }
    }
    else
    {
      std::cerr << "Lock failed" << std::endl;
    }
  }
  catch(...)
  {
    exception::Log("Swallowed exception");
  }
}

void InitChannelMixer()
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
void Free()
{
  chunks.clear();
}

void Free(boost::filesystem::path const& file)
{
  chunks.erase(file.string());
}

void Sound::Impl::Play()
{
  if(channel_ == -1)
  {
    if(Mix_Playing(-1) < 256)
    {
      channel_ = Mix_PlayChannel(-1, chunk_.get(), loops_);
      if(channel_ != -1)
      {
        Mix_Volume(channel_, volume_);
        std::lock_guard<std::mutex> lock(mutex);
        channels.emplace(channel_, shared_from_this());
      }
      else
      {
        BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
      }
    }
  }
}

Sound::Impl::Impl(boost::filesystem::path const& file, float volume, bool repeat) : channel_(-1), loops_(repeat ? -1 : 0), volume_(int(volume / MIX_MAX_VOLUME)), play_on_resume_(true)
{
  InitChannelMixer();

  auto fileiter = chunks.find(file.string());
  if(fileiter != chunks.end())
  {
    chunk_ = fileiter->second;
  }
  else
  {
    chunk_ = Chunk(Init(file), Mix_FreeChunk);
    chunks.emplace(file.string(), chunk_);
  }
}

Sound::Impl::~Impl()
{
  if(channel_ != -1)
  {
    Mix_HaltChannel(channel_);
  }
}

void Sound::Impl::Pause()
{
  if(channel_ != -1)
  {
    Mix_Pause(channel_);
  }
}

void Sound::Impl::Resume()
{
  if(play_on_resume_)
  {
    play_on_resume_ = false;
    Play();
  }
  else
  {
    if(channel_ != -1)
    {
      Mix_Resume(channel_);
    }
  }
}

Sound::Sound(boost::filesystem::path const& file, float volume, bool repeat) : impl_(std::make_shared<Impl>(file, volume, repeat))
{
}

void Sound::Pause()
{
  impl_->Pause();
}

void Sound::Resume()
{
  impl_->Resume();
}

bool Sound::operator()()
{
  bool valid = false;
  if(impl_)
  {
    impl_->Play();
    valid = true;
  }
  return valid;
}

Sound::operator bool() const
{
  return bool(impl_) && (impl_->channel_ != -1);
}
}
