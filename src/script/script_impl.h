#ifndef SCRIPT_IMPL_H_
#define SCRIPT_IMPL_H_
#include "script.h"
#include "actor.h"
#include "scenery.h"
#include "timer.h"
#include "lua_stack.h"
#include "choice.h"
#include "scene.h"
#include "screen.h"
#include <memory>
#include <map>
#include <string>
#include <list>
#include "boost/optional.hpp"
#include <array>
#include "subtitle.h"
#include "fade.h"
#include "signal.h"
#include "boost/filesystem.hpp"
#include "game_joint.h"
#include "music.h"
#include "sound.h"
namespace game
{
typedef std::list<Actor::WeakPtr> ActorList;
typedef boost::optional<Position> Subject;
typedef std::multimap<std::string, Joint> JointMap;
typedef std::multimap<std::string, Scenery> SceneryMap;
typedef std::multimap<std::string, Actor> ActorMap;
typedef std::multimap<std::string, event::Timer> TimerMap;
typedef std::multimap<std::string, Screen> ScreenMap;
typedef std::multimap<std::string, audio::Sound> SoundMap;
typedef std::map<std::string, audio::Music> MusicMap;
typedef std::array<bool, 2> Paused;

struct Stage
{
  Actor hero_;
  JointMap joints_;
  SceneryMap scenery_;
  ActorMap actors_;
  Paused paused_;
  TimerMap timers_;
  ScreenMap screens_;
  SoundMap sounds_;
  MusicMap music_;
  audio::Music current_music_;
  Subject subject_;
  ActorList subjects_;
  float zoom_;

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
  void Init(boost::filesystem::path const& file);
  
  void Pause();
  void Resume();
  void Render();

  void ChoiceUp();
  void ChoiceDown();
  void ChoiceLeft();
  void ChoiceRight();
  void Up();
  void Down();
  void Left();
  void Right(); 

  void Call(std::string const& call);
  void View(dynamics::World::WeakPtr world);
  void Pause(StagePtr const& stage, bool& paused);
  bool Pause(StagePtr const& stage);
  void Resume(StagePtr const& stage, bool& paused);
  void Add(event::Command const& command);

  void ActorInit();
  void ActorLoad();
  void ActorFree();
  void ActorBody();
  void ActorEyes();
  void ActorMouth();
  void ActorNominate();
  void ActorPosition();
  void ActorVelocity();
  void ActorForce();
  void ActorImpulse();
  void ActorUp();
  void ActorDown();
  void ActorLeft();
  void ActorRight();

  void SceneryInit();
  void SceneryLoad();
  void SceneryFree();
  void SceneryLight();

  void ScreenInit();
  void ScreenLoad();
  void ScreenFree();
  void ScreenLight();

  void StageInit();
  void StageLoad();
  void StageFree();
  void StageLight();
  void StageNominate();
  void StagePause();
  void StageResume();
  StagePtr StagePop();

  void ChoiceInit();
  void ChoiceChoice();

  void SubtitleInit();
  void SubtitleText();
  void SubtitleLight();

  void TimerInit();
  void TimerLoad();
  void TimerFree();

  void ViewInit();
  void ViewAddActor();
  void ViewActor();
  void ViewPoint();
  void ViewZoom();

  void CollisionInit();
  void CollisionBegin();
  void CollisionEnd();

  void FadeInit();
  void FadeUp();
  void FadeDown();
  void FadeEnd();

  void JointInit();
  void JointLoad();
  void JointFree();

  void AudioInit();
  void SoundLoad();
  void SoundFree();
  void SoundPlay();
  void MusicLoad();
  void MusicFree();
  void MusicPlay();

  void ResourceInit();
  void ResourceCollect();

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
};
}
#endif