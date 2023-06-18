#include <map>
#include "music.h"
#include "SDL_mixer.h"
#include "mix_library.h"
#include "mix_exception.h"
#include "log.h"
#include "mix_thread.h"
#include <mutex>
#include <boost/flyweight.hpp>
#include <boost/flyweight/key_value.hpp>

namespace
{
struct StreamContainer
{
  Mix_Music* stream_;

  StreamContainer(boost::filesystem::path const& file)
  {
    stream_ = Mix_LoadMUS(file.string().c_str());
    if(!stream_)
    {
      BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
    }
  }

  ~StreamContainer()
  {
    Mix_FreeMusic(stream_);
  }

  operator Mix_Music*() const
  {
    return stream_;
  }
};

typedef boost::flyweight<boost::flyweights::key_value<boost::filesystem::path, StreamContainer>> Stream;
}

namespace audio
{
class Music::Impl
{
public:
  Impl(boost::filesystem::path const& file, float volume, bool repeat);
  ~Impl();
  auto Play(float volume) -> void;
  auto Pause() -> void;
  auto Resume() -> void;
  auto End() -> void;
  mix::Library mix_;
  Stream stream_;
  float play_volume_;
  float volume_;
  bool repeat_;
  bool complete_;
  bool paused_;
  bool end_;
};
}

namespace
{
audio::Music::Impl* music;

auto MusicFinished() noexcept -> void
{
  try
  {
    std::lock_guard<mix::Mutex> lock(mix::mutex);
    if(music)
    {
      audio::Music::Impl* temp = music;
      music = nullptr;
      if(temp)
      {
        temp->complete_ = true;
        if(temp->repeat_ && !temp->end_)
        {
          temp->Play(temp->play_volume_);
        }
      }
    }
  }
  catch(...)
  {
    exception::Log("Swallowed exception");
  }
}

auto InitHookMusic() -> void
{
  static bool initialised;
  if(!initialised)
  {
    initialised = true;
    Mix_HookMusicFinished(MusicFinished);
  }
}
}

namespace audio
{
auto Music::Impl::Pause() -> void
{
  std::lock_guard<mix::Mutex> lock(mix::mutex);
  paused_ = true;
  if(music == this)
  {
    Mix_PauseMusic();
  }
}

auto Music::Impl::Resume() -> void
{
  std::lock_guard<mix::Mutex> lock(mix::mutex);
  paused_ = false;
  if(music == this)
  {
    Mix_ResumeMusic();
  }
}

auto Music::Impl::End() -> void
{
  std::lock_guard<mix::Mutex> lock(mix::mutex);
  end_ = true;
}

auto Music::Impl::Play(float volume) -> void
{
  std::lock_guard<mix::Mutex> lock(mix::mutex);
  play_volume_ = volume;
  if(music != this)
  {
    complete_ = false;
    if(Mix_PlayMusic(stream_.get(), 0) == -1)
    {
      BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
    }
    if(paused_)
    {
      Mix_PauseMusic();
    }
    music = this;
  }
  Mix_VolumeMusic(static_cast<int>(play_volume_ * volume_ * MIX_MAX_VOLUME));
  end_ = false;
}

Music::Impl::Impl(boost::filesystem::path const& file, float volume, bool repeat) : repeat_(repeat), play_volume_(1.f), volume_(volume), complete_(false), paused_(true), end_(false),  stream_(file)
{
  InitHookMusic();
}

Music::Impl::~Impl()
{
  std::lock_guard<mix::Mutex> lock(mix::mutex);
  if(music == this)
  {
    end_ = true;
    Mix_HaltMusic();
  }
}

Music::Music(boost::filesystem::path const& file, float volume, bool repeat) : impl_(std::make_shared<Impl>(file, volume, repeat))
{
}

Music::Music(lua::Stack& lua, boost::filesystem::path const& path) : Music(path / lua.Field<std::string>("file"), lua.Field<float>("volume"), lua.Field<bool>("repeat"))
{
}

auto Music::operator()(float volume) -> bool
{
  bool valid = static_cast<bool>(impl_);
  if(valid)
  {
    impl_->Play(volume);
  }
  else
  {
    std::lock_guard<mix::Mutex> lock(mix::mutex);
    if(music)
    {
      music->end_ = true;
    }
    Mix_HaltMusic();
  }
  return valid;
}

Music::operator bool() const
{
  bool valid = static_cast<bool>(impl_);
  if(valid)
  {
    std::lock_guard<mix::Mutex> lock(mix::mutex);
    valid = !impl_->complete_;
  }
  return valid;
}

auto Music::Pause() -> void
{
  impl_->Pause();
}

auto Music::Resume() -> void
{
  impl_->Resume();
}

auto Music::End() -> void
{
  impl_->End();
}

size_t Music::Hash(Music const& music)
{
  return std::hash<std::shared_ptr<Impl>>()(music.impl_);
}

bool Music::operator==(Music const& other) const
{
  return impl_ == other.impl_;
}
}