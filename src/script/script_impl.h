#ifndef SCRIPT_IMPL_H_
#define SCRIPT_IMPL_H_
#include "script.h"
#include "actor.h"
#include "timer.h"
#include "lua_stack.h"
#include "choice.h"
#include "scene.h"
#include <memory>
#include <map>
#include <string>
#include <list>
#include <array>
#include "subtitle.h"
#include "fade.h"
#include "signal.h"
#include "boost/filesystem.hpp"
#include "game_joint.h"
#include "music.h"
#include "sound.h"
#include "timer.h"
namespace game
{
typedef std::list<Actor::WeakPtr> ActorList;
typedef std::multimap<std::string, Joint> JointMap;
typedef std::multimap<std::string, Actor> ActorMap;
typedef std::multimap<std::string, event::Timer> TimerMap;
typedef std::multimap<std::string, audio::Sound> SoundMap;
typedef std::map<std::string, audio::Music> MusicMap;
typedef std::array<bool, 2> Paused;

struct Stage
{
  JointMap joints_;
  ActorMap actors_;
  Paused paused_;
  TimerMap timers_;
  SoundMap sounds_;
  MusicMap music_;
  audio::Music current_music_;
  ActorList subjects_;
  float zoom_;
  double angle_;

  Scene scene_;
  Choice choice_;
  Subtitle subtitle_;

  dynamics::World world_;
  collision::Collision collision_;
  collision::Group group_;
};

typedef std::shared_ptr<Stage> StagePtr;
typedef std::map<std::string, StagePtr> StageMap;
typedef std::pair<std::string, StagePtr> StagePair;

class Script::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(display::Window& window, event::Queue& queue, boost::filesystem::path const& path, float volume);
  auto Init(boost::filesystem::path const& file) -> void;
  
  auto Pause() -> void;
  auto Resume() -> void;
  auto Render() -> void;

  auto ChoiceUp() -> void;
  auto ChoiceDown() -> void;
  auto ChoiceLeft() -> void;
  auto ChoiceRight() -> void;
  auto Control(float x, float y) -> void;
  auto Call(std::string const& call) -> void;
  auto View() -> void;
  auto Pause(StagePtr const& stage, bool& paused) -> void;
  auto Pause(StagePtr const& stage) -> bool;
  auto Resume(StagePtr const& stage, bool& paused) -> void;
  auto Add(event::Command const& command) -> void;
  
  auto Collect() -> void;

  auto ActorInit() -> void;
  auto ActorLoad() -> void;
  auto ActorFree() -> void;
  auto ActorBody() -> void;
  auto ActorEyes() -> void;
  auto ActorMouth() -> void;
  auto ActorPosition() -> void;
  auto ActorVelocity() -> void;
  auto ActorForce() -> void;
  auto ActorImpulse() -> void;
  auto ActorModulation() -> void;
  auto ActorDilation() -> void;
  auto ActorRotation() -> void;

  auto SceneryInit() -> void;
  auto SceneryLoad() -> void;
  auto SceneryFree() -> void;
  auto SceneryModulation() -> void;

  auto ScreenInit() -> void;
  auto ScreenLoad() -> void;
  auto ScreenFree() -> void;
  auto ScreenModulation() -> void;

  auto StageInit() -> void;
  auto StageLoad() -> void;
  auto StageFree() -> void;
  auto StageAmbient() -> void;
  auto StageNominate() -> void;
  auto StagePause() -> void;
  auto StageResume() -> void;
  auto StageGet() -> StagePtr;

  auto ChoiceInit() -> void;
  auto ChoiceChoice() -> void;
  auto ChoiceUpModulation() -> void;
  auto ChoiceDownModulation() -> void;
  auto ChoiceLeftModulation() -> void;
  auto ChoiceRightModulation() -> void;

  auto SubtitleInit() -> void;
  auto SubtitleText() -> void;
  auto SubtitleModulation() -> void;

  auto TimerInit() -> void;
  auto TimerLoad() -> void;
  auto TimerFree() -> void;

  auto ViewInit() -> void;
  auto ViewAddActor() -> void;
  auto ViewActor() -> void;
  auto ViewZoom() -> void;
  auto ViewRotation() -> void;

  auto CollisionInit() -> void;
  auto CollisionBegin() -> void;
  auto CollisionEnd() -> void;

  auto FadeInit() -> void;
  auto FadeUp() -> void;
  auto FadeDown() -> void;
  auto FadeEnd() -> void;

  auto JointInit() -> void;
  auto JointLoad() -> void;
  auto JointFree() -> void;

  auto AudioInit() -> void;
  auto SoundLoad() -> void;
  auto SoundFree() -> void;
  auto SoundPlay() -> void;
  auto SoundEnd() -> void;
  auto MusicLoad() -> void;
  auto MusicFree() -> void;
  auto MusicPlay() -> void;
  auto MusicEnd() -> void;

  lua::Stack lua_;
  display::Window window_;
  event::Queue queue_;
  StageMap stages_;
  StagePair stage_;
  bool paused_;
  Fade fade_;
  event::Signal signal_;
  bool begun_;
  boost::filesystem::path path_;
  float volume_;
  event::Timer collect_;
};
}
#endif