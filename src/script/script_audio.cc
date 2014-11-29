#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::AudioInit()
{
  lua_.Add(function::Bind(&Impl::SoundLoad, shared_from_this()), "sound_load", 0);
  lua_.Add(function::Bind(&Impl::SoundFree, shared_from_this()), "sound_free", 0);
  lua_.Add(function::Bind(&Impl::SoundPlay, shared_from_this()), "sound_play", 0);
  lua_.Add(function::Bind(&Impl::MusicLoad, shared_from_this()), "music_load", 0);
  lua_.Add(function::Bind(&Impl::MusicFree, shared_from_this()), "music_free", 0);
  lua_.Add(function::Bind(&Impl::MusicPlay, shared_from_this()), "music_play", 0);
}

void Script::Impl::SoundLoad()
{
  StagePtr stage = StagePop();
  std::string name;
  std::string file;
  lua_.PopFront(name);
  lua_.PopFront(file);
  if(stage)
  {
    audio::Sound sound(json::JSON(path_ / file), path_);
    if(!Pause(stage))
    {
      sound.Resume();
    }
    stage->sounds_.emplace(name, sound);
  }
}

void Script::Impl::SoundFree()
{
  StagePtr stage = StagePop();
  std::string name;
  lua_.PopFront(name);
  if(stage)
  {
    stage->sounds_.erase(name);
  }
}

void Script::Impl::SoundPlay()
{
  StagePtr stage = StagePop();
  std::string name;
  lua_.PopFront(name);
  if(stage)
  {
    float volume = 0.f;
    if(stage == stage_.second)
    {
      volume = volume_;
    }

    auto range = stage->sounds_.equal_range(name);
    for(auto& sound = range.first; sound != range.second; ++sound)
    {
      sound->second(volume_);
    }
  }
}

void Script::Impl::MusicLoad()
{
  StagePtr stage = StagePop();
  std::string name;
  std::string file;
  lua_.PopFront(name);
  lua_.PopFront(file);
  if(stage)
  {
    audio::Music music(json::JSON(path_ / file), path_);
    if(!Pause(stage))
    {
      music.Resume();
    }
    stage->music_.emplace(name, music);
  }
}

void Script::Impl::MusicFree()
{
  StagePtr stage = StagePop();
  std::string name;
  lua_.PopFront(name);
  if(stage)
  {
    stage->music_.erase(name);
  }
}

void Script::Impl::MusicPlay()
{
  StagePtr stage = StagePop();
  std::string name;
  lua_.PopFront(name);
  if(stage)
  {
    auto iter = stage->music_.find(name);
    if(iter != stage->music_.end())
    {
      stage->current_music_ = iter->second;
    }
    else
    {
      stage->current_music_ = audio::Music();
    }

    if(stage == stage_.second)
    {
      stage->current_music_(volume_);
    }
  }
}
}