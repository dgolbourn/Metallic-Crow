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
  void Play(float volume);
  void Pause();
  void Resume();
  void End();
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

void MusicFinished() noexcept
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

std::unordered_map<boost::filesystem::path, Stream, boost::hash<boost::filesystem::path>> cache;

std::unordered_map<boost::filesystem::path, WeakStream, boost::hash<boost::filesystem::path>> streams;

Stream MakeStream(boost::filesystem::path const& file)
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
void Music::Load(boost::filesystem::path const& file)
{
  cache.emplace(file, MakeStream(file));
}

void Music::Free(boost::filesystem::path const& file)
{
  cache.erase(file.string());
}

void Music::Impl::Pause()
{
  std::lock_guard<mix::Mutex> lock(mix::mutex);
  paused_ = true;
  if(music == this)
  {
    Mix_PauseMusic();
  }
}

void Music::Impl::Resume()
{
  std::lock_guard<mix::Mutex> lock(mix::mutex);
  paused_ = false;
  if(music == this)
  {
    Mix_ResumeMusic();
  }
}

void Music::Impl::End()
{
  std::lock_guard<mix::Mutex> lock(mix::mutex);
  end_ = true;
}

void Music::Impl::Play(float volume)
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
  Mix_VolumeMusic(int(play_volume_ * volume_ * MIX_MAX_VOLUME));
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

bool Music::operator()(float volume)
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

void Music::Pause()
{
  impl_->Pause();
}

void Music::Resume()
{
  impl_->Resume();
}

void Music::End()
{
  impl_->End();
}
}