#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::AudioInit()
{
  lua_.Add(function::Bind(&Impl::SoundLoad, shared_from_this()), "sound_load", 0);
  lua_.Add(function::Bind(&Impl::SoundFree, shared_from_this()), "sound_free", 0);
  lua_.Add(function::Bind(&Impl::SoundPlay, shared_from_this()), "sound_play", 0);
  lua_.Add(function::Bind(&Impl::SoundEnd, shared_from_this()), "sound_end", 0);
  lua_.Add(function::Bind(&Impl::MusicLoad, shared_from_this()), "music_load", 0);
  lua_.Add(function::Bind(&Impl::MusicFree, shared_from_this()), "music_free", 0);
  lua_.Add(function::Bind(&Impl::MusicPlay, shared_from_this()), "music_play", 0);
  lua_.Add(function::Bind(&Impl::MusicEnd, shared_from_this()), "music_end", 0);
}

void Script::Impl::SoundLoad()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-3);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(name);
    }
    
    audio::Sound sound;
    {
      lua::Guard guard = lua_.Get(-1);
      sound = audio::Sound(lua_, path_);
    }

    if(!Pause(stage))
    {
      sound.Resume();
    }
    stage->sounds_.emplace(name, sound);
  }
}

void Script::Impl::SoundFree()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(name);
    }
    
    stage->sounds_.erase(name);
  }
}

void Script::Impl::SoundPlay()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(name);
    }
    
    float volume = 0.f;
    if(stage == stage_.second)
    {
      volume = volume_;
    }

    auto range = stage->sounds_.equal_range(name);
    for(auto& sound = range.first; sound != range.second; ++sound)
    {
      sound->second(volume);
    }
  }
}

void Script::Impl::SoundEnd()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(name);
    }

    auto range = stage->sounds_.equal_range(name);
    for(auto& sound = range.first; sound != range.second; ++sound)
    {
      sound->second.End();
    }
  }
}

void Script::Impl::MusicLoad()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-3);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(name);
    }
    
    audio::Music music;
    {
      lua::Guard guard = lua_.Get(-1);
      music = audio::Music(lua_, path_);
    }

    if(!Pause(stage))
    {
      music.Resume();
    }
    stage->music_.emplace(name, music);
  }
}

void Script::Impl::MusicFree()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(name);
    }
    stage->music_.erase(name);
  }
}

void Script::Impl::MusicEnd()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(name);
    }
    auto iter = stage->music_.find(name);
    if(iter != stage->music_.end())
    {
      iter->second.End();
    }
  }
}

void Script::Impl::MusicPlay()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(name);
    }
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