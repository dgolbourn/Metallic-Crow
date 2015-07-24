#include "script_impl.h"
#include "bind.h"
namespace game
{
auto Script::Impl::AudioInit() -> void
{
  lua::Init<std::pair<WeakStagePtr, audio::Sound::WeakPtr>>(static_cast<lua_State*>(lua_));
  lua_.Add(function::Bind(&Impl::SoundLoad, shared_from_this()), "sound_load", 1, "metallic_crow");
  lua_.Add(function::Bind(&Impl::SoundFree, shared_from_this()), "sound_free", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::SoundPlay, shared_from_this()), "sound_play", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::SoundEnd, shared_from_this()), "sound_end", 0, "metallic_crow");
  lua::Init<std::pair<WeakStagePtr, audio::Music::WeakPtr>>(static_cast<lua_State*>(lua_));
  lua_.Add(function::Bind(&Impl::MusicLoad, shared_from_this()), "music_load", 1, "metallic_crow");
  lua_.Add(function::Bind(&Impl::MusicFree, shared_from_this()), "music_free", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::MusicPlay, shared_from_this()), "music_play", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::MusicEnd, shared_from_this()), "music_end", 0, "metallic_crow");
}

auto Script::Impl::SoundLoad() -> void
{
  audio::Sound sound;

  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    {
      lua::Guard guard = lua_.Get(-1);
      sound = audio::Sound(lua_, path_);
    }
    if(!Pause(stage))
    {
      sound.Resume();
    }
    stage->sounds_.emplace(sound);
  }

  lua::Push(static_cast<lua_State*>(lua_), std::pair<WeakStagePtr, audio::Sound::WeakPtr>(stage, sound));
}

auto Script::Impl::SoundFree() -> void
{
  std::pair<StagePtr, audio::Sound> sound = StageDataGet<audio::Sound>();
  if(sound.first && sound.second)
  {
    sound.first->sounds_.erase(sound.second);
  }
}

auto Script::Impl::SoundPlay() -> void
{
  std::pair<StagePtr, audio::Sound> sound = StageDataGet<audio::Sound>();
  if(sound.first && sound.second)
  { 
    if(sound.first == stage_)
    {
      sound.second(volume_);
    }
    else
    {
      sound.second(0.f);
    }
  }
}

auto Script::Impl::SoundEnd() -> void
{
  audio::Sound sound = DataGet<audio::Sound>();
  if(sound)
  {
    sound.End();
  }
}

auto Script::Impl::MusicLoad() -> void
{
  audio::Music music;

  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    {
      lua::Guard guard = lua_.Get(-1);
      music = audio::Music(lua_, path_);
    }
    if(!Pause(stage))
    {
      music.Resume();
    }
    stage->music_.emplace(music);
  }

  lua::Push(static_cast<lua_State*>(lua_), std::pair<WeakStagePtr, audio::Music::WeakPtr>(stage, music));
}

auto Script::Impl::MusicFree() -> void
{
  std::pair<StagePtr, audio::Music> music = StageDataGet<audio::Music>();
  if(music.first && music.second)
  {
    music.first->music_.erase(music.second);
  }
}

auto Script::Impl::MusicEnd() -> void
{
  audio::Music music = DataGet<audio::Music>();
  if(music)
  {
    music.End();
  }
}

auto Script::Impl::MusicPlay() -> void
{
  std::pair<StagePtr, audio::Music> music = StageDataGet<audio::Music>();
  if(music.first)
  {
    music.first->current_music_ = music.second;
    if(music.first == stage_)
    {
      stage_->current_music_(volume_);
    }
  }
}
}