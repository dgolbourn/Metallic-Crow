#include "script.h"
#include "music.h"
#include "hero.h"
#include "box.h"
#include "item.h"
#include "bind.h"
#include <vector>
#include "terrain.h"
#include "timer.h"
#include "lua.hpp"
#include "lua_library.h"
#include "lua_exception.h"
namespace game
{
class ScriptImpl final : public std::enable_shared_from_this<ScriptImpl>
{
public:
  ScriptImpl(display::Window& window, Subtitle& subtitle, event::Queue& queue);
  void Init(json::JSON const& json, Subtitle& subtitle, display::Window& window, Scene& scene, event::Queue& queue, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world, event::Event& event);
  void Pause(void);
  void Resume(void);
  void View(void);
  void Event(std::string const& edge);
  bool paused_;
  lua::Library lua_;
  audio::Music music_;
  std::vector<Item> items_;
  std::vector<Box> boxes_;
  std::vector<Terrain> terrain_;
  Hero hero_;
  display::Window window_;
  Subtitle subtitle_;
  event::Timer timer_;
  event::Queue queue_;
  Position focus_;
  bool subject_focus_;
  bool subject_hero_;
  float zoom_;
};

ScriptImpl::ScriptImpl(display::Window& window, Subtitle& subtitle, event::Queue& queue) : paused_(true), window_(window), subtitle_(subtitle), queue_(queue), zoom_(1.f), focus_(0.f, 0.f), subject_hero_(false), subject_focus_(false)
{
}

void ScriptImpl::Init(json::JSON const& json, Subtitle& subtitle, display::Window& window, Scene& scene, event::Queue& queue, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world, event::Event& event)
{
  auto ptr = shared_from_this();
  world.Add(event::Bind(&ScriptImpl::View, ptr));
  subtitle_.Up(event::Bind(&ScriptImpl::Event, ptr, "up"));
  subtitle_.Down(event::Bind(&ScriptImpl::Event, ptr, "down"));
  subtitle_.Left(event::Bind(&ScriptImpl::Event, ptr, "left"));
  subtitle_.Right(event::Bind(&ScriptImpl::Event, ptr, "right"));

  std::string story_state("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/story.lua");
  lua_.Load(story_state);

  audio::Music music("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/BassRockinDJJin-LeeRemix.mp3");
  music.Volume(0.01);
  music.Play();
  music.Pause();
  music_ = music;

  Hero hero(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/hero.json"), window, scene, dcollision, ccollision, queue, world, event);
  hero_ = hero;

  terrain_.push_back(Terrain(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/skyline.json"), window, scene, dcollision, world));
  terrain_.push_back(Terrain(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/boundary.json"), window, scene, dcollision, world));
  terrain_.push_back(Terrain(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/bushes.json"), window, scene, dcollision, world));
  terrain_.push_back(Terrain(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/grass.json"), window, scene, dcollision, world));
  terrain_.push_back(Terrain(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/tree1.json"), window, scene, dcollision, world));
  terrain_.push_back(Terrain(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/tree2.json"), window, scene, dcollision, world));

  {
    Item crow(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/crow.json"), window, scene, queue, ccollision, world);
    crow.Hysteresis(event::Bind(&ScriptImpl::Event, ptr, "crow"), event::Bind(&ScriptImpl::Event, ptr, "exit"));
    items_.push_back(crow);
  }

  {
    Item house(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/house.json"), window, scene, queue, ccollision, world);
    house.Hysteresis(event::Bind(&ScriptImpl::Event, ptr, "house"), event::Bind(&ScriptImpl::Event, ptr, "exit"));
    items_.push_back(house);
  }

  {
    Item rooster(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/rooster.json"), window, scene, queue, ccollision, world);
    rooster.Hysteresis(event::Bind(&ScriptImpl::Event, ptr, "rooster"), event::Bind(&ScriptImpl::Event, ptr, "exit"));
    items_.push_back(rooster);
  }
}

void ScriptImpl::Event(std::string const& event)
{
  lua_getglobal(lua_, "event");
  lua_pushstring(lua_, event.c_str());
  lua_.Call(1, 11);

  std::string text;
  lua_.Pop(text);
  subtitle_.Text(text);

  std::string up;
  lua_.Pop(up);
  std::string down;
  lua_.Pop(down);
  std::string left;
  lua_.Pop(left);
  std::string right;
  lua_.Pop(right);
  subtitle_.Choice(up, down, left, right);

  int timer;
  lua_.Pop(timer);
  if(timer > 0)
  {
    timer_ = event::Timer(timer, queue_);
    timer_.Add(event::Bind(&ScriptImpl::Event, shared_from_this(), "timer"));
    timer_.Play(0);
  }
  else
  {
    timer_ = event::Timer();
  }

  lua_.Pop(focus_.first);
  lua_.Pop(focus_.second);
  lua_.Pop(zoom_);
  lua_.Pop(subject_focus_);
  lua_.Pop(subject_hero_);
}

void ScriptImpl::View(void)
{
  game::Position p;
  if(subject_hero_)
  {
    p = hero_.Position();
    if(subject_focus_)
    {
      p.first += focus_.first;
      p.first *= .5f;
      p.second += focus_.second;
      p.second *= .5f;
    }
  }
  else if(subject_focus_)
  {
    p = focus_;
  }
  else
  {
    p = hero_.Position();
    p.second -= 200.f;
  }
  window_.View(p.first, p.second, zoom_);
}

void ScriptImpl::Pause(void)
{
  if(!paused_)
  {
    paused_ = true;
    music_.Pause();
    for(auto& item : items_)
    {
      item.Pause();
    }
    for(auto& box : boxes_)
    {
      box.Pause();
    }
    hero_.Pause();
    if(timer_)
    {
      timer_.Pause();
    }
  }
}

void ScriptImpl::Resume(void)
{
  if(paused_)
  {
    paused_ = false;
    music_.Resume();
    for(auto& item : items_)
    {
      item.Resume();
    }
    for(auto& box : boxes_)
    {
      box.Resume();
    }
    hero_.Resume();
    if(timer_)
    {
      timer_.Resume();
    }
  }
}

void Script::Pause(void)
{
  impl_->Pause();
}

void Script::Resume(void)
{
  impl_->Resume();
}

Script::Script(json::JSON const& json, Subtitle& subtitle, display::Window& window, Scene& scene, event::Queue& queue, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world, event::Event& event)
{
  impl_ = std::make_shared<ScriptImpl>(window, subtitle, queue);
  impl_->Init(json, subtitle, window, scene, queue, dcollision, ccollision, world, event);
}
}