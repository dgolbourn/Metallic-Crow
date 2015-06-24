#include <map>
#include "music.h"
#include "SDL_mixer.h"
#include "mix_library.h"
#include "mix_exception.h"
#include <unordered_map>
#include "boost/functional/hash.hpp"
#include "log.h"
#include "mix_thread.h"
#include <mutex>
namespace
{
typedef std::shared_ptr<Mix_Music> Stream;
typedef std::weak_ptr<Mix_Music> WeakStream;
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

auto Init(boost::filesystem::path const& file) -> Mix_Music*
{
  Mix_Music* music = Mix_LoadMUS(file.string().c_str());
  if(!music)
  {
    BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
  }
  return music;
}

std::unordered_map<boost::filesystem::path, Stream, boost::hash<boost::filesystem::path>> cache;

std::unordered_map<boost::filesystem::path, WeakStream, boost::hash<boost::filesystem::path>> streams;

auto MakeStream(boost::filesystem::path const& file) -> Stream
{
  Stream stream;
  auto fileiter = streams.find(file);
  if(fileiter != streams.end())
  {
    stream = fileiter->second.lock();
  }
  if(!stream)
  {
    stream = Stream(Init(file), Mix_FreeMusic);
    streams.emplace(file, stream);
  }
  return stream;
}
}

namespace audio
{
auto Music::Load(boost::filesystem::path const& file) -> void
{
  cache.emplace(file, MakeStream(file));
}

auto Music::Free(boost::filesystem::path const& file) -> void
{
  cache.erase(file.string());
}

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

Music::Impl::Impl(boost::filesystem::path const& file, float volume, bool repeat) : repeat_(repeat), play_volume_(1.f), volume_(volume), complete_(false), paused_(true), end_(false)
{
  InitHookMusic();

  stream_ = MakeStream(file);
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

Music::Music(lua::Stack& lua, boost::filesystem::path const& path)
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

auto Music::operator()(float volume) -> bool
{
  bool valid = bool(impl_);
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
  bool valid = bool(impl_);
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
}