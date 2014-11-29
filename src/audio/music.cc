#include <map>
#include "music.h"
#include "SDL_mixer.h"
#include "mix_library.h"
#include "mix_exception.h"
namespace audio
{
class Music::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(boost::filesystem::path const& file, float volume, bool repeat);
  ~Impl();
  void Play();
  void Pause();
  void Resume();
  mix::Library mix_;
  Mix_Music* music_;
  int volume_;
  int loops_;
  bool complete_;
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

void Init()
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
void Music::Impl::Pause()
{
  if(music.lock() == shared_from_this())
  {
    Mix_PauseMusic();
  }
}

void Music::Impl::Resume()
{
  if(music.lock() == shared_from_this())
  {
    Mix_ResumeMusic();
  }
}

void Music::Impl::Play()
{
  complete_ = false;
  if(music.lock() == shared_from_this())
  {
    Mix_RewindMusic();
    Mix_ResumeMusic();
  }
  else
  {
    if(Mix_PlayMusic(music_, loops_) == -1)
    {
      BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
    }
    Mix_VolumeMusic(volume_);
    music = shared_from_this();
  }
}

Music::Impl::Impl(boost::filesystem::path const& file, float volume, bool repeat) : loops_(repeat ? -1 : 0), volume_(int(volume / MIX_MAX_VOLUME)), complete_(false)
{
  Init();
  music_ = Mix_LoadMUS(file.string().c_str());
  if(!music_)
  {
    BOOST_THROW_EXCEPTION(mix::Exception() << mix::Exception::What(mix::Error()));
  }
}

Music::Impl::~Impl()
{
  Mix_FreeMusic(music_);
}

Music::Music(boost::filesystem::path const& file, float volume, bool repeat) : impl_(std::make_shared<Impl>(file, volume, repeat))
{
}

bool Music::operator()()
{
  bool valid = false;
  if(impl_)
  {
    impl_->Play();
    valid = true;
  }
  return valid;
}

Music::operator bool(void) const
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