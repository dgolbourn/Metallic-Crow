#ifndef SCRIPT_IMPL_H_
#define SCRIPT_IMPL_H_
#include "script.h"
#include "actor.h"
#include "terrain.h"
#include "timer.h"
#include "lua_stack.h"
#include "choice.h"
#include "scene.h"
#include <memory>
#include <map>
#include <string>
#include <list>
#include "boost/optional.hpp"
#include <array>
#include "subtitle.h"
namespace game
{
typedef std::list<Actor::WeakPtr> ActorList;
typedef boost::optional<Position> Subject;
typedef std::multimap<std::string, Terrain> TerrainMap;
typedef std::multimap<std::string, Actor> ActorMap;
typedef std::multimap<std::string, event::Timer> TimerMap;
typedef std::array<bool, 2> Paused;

struct Stage
{
  Actor hero_;
  TerrainMap terrain_;
  ActorMap actors_;
  Paused paused_;
  TimerMap timers_;
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

class Script::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(display::Window& window, event::Queue& queue);
  void Init(std::string const& file);
  
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

  void TerrainInit();
  void TerrainLoad();
  void TerrainFree();
  void TerrainLight();

  void StageInit();
  void StageLoad();
  void StageFree();
  void StageLight();
  void StageNominate();
  void StagePause();
  void StageResume();

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
  void CollisionLink();
  void CollisionBegin();
  void CollisionEnd();

  lua::Stack lua_;
  display::Window window_;
  event::Queue queue_;
  StageMap stages_;
  StagePtr stage_;
  bool paused_;
};
}
#endif