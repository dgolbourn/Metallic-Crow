#include "script.h"
#include "music.h"
#include "hero.h"
#include "box.h"
#include "item.h"
#include "bind.h"
#include <vector>
#include "terrain.h"
#include "timer.h"
#include "lua_stack.h"
#include "exception.h"
#include "subtitle.h"
#include <map>
namespace game
{
typedef std::multimap<std::string, Item> ItemMap;
typedef std::multimap<std::string, Box> BoxMap;
typedef std::multimap<std::string, Terrain> TerrainMap;

struct Stage
{
  audio::Music music_;
  Hero hero_;
  game::Scene scene_;
  dynamics::World world_;
  ItemMap items_;
  BoxMap boxes_;
  TerrainMap terrain_;
};

typedef std::shared_ptr<Stage> StagePtr;
typedef std::map<std::string, StagePtr> StageMap;

class ScriptImpl final : public std::enable_shared_from_this<ScriptImpl>
{
public:
  ScriptImpl(display::Window& window, event::Queue& queue);
  void Init(json::JSON const& json);
  void Pause(void);
  void Resume(void);
  void Render(void);
  void ChoiceUp(void);
  void ChoiceDown(void);
  void ChoiceLeft(void);
  void ChoiceRight(void);
  void Up(void);
  void Down(void);
  void Left(void);
  void Right(void);
  void View(void);
  void Event(std::string const& event);
  void Command(void);
  bool paused_;
  lua::Stack lua_;
  display::Window window_;
  Subtitle subtitle_;
  event::Timer timer_;
  event::Queue queue_;
  Position focus_;
  bool subject_focus_;
  bool subject_hero_;
  float zoom_;
  game::Collision collision_;
  game::DynamicsCollision dcollision_;
  game::CommandCollision ccollision_;
  StageMap stages_;
  StagePtr stage_;
};

ScriptImpl::ScriptImpl(display::Window& window, event::Queue& queue) : paused_(true), window_(window), zoom_(1.f), focus_(0.f, 0.f), subject_hero_(false), subject_focus_(false), queue_(queue)
{
  collision_ = game::Collision(queue_);
  dcollision_ = game::DynamicsCollision(collision_);
  ccollision_ = game::CommandCollision(collision_);
}

void ScriptImpl::Init(json::JSON const& json)
{
  json_t* subtitle;
  char const* story;

  json.Unpack("{soss}",
    "subtitle", &subtitle,
    "story", &story);

  subtitle_ = game::Subtitle(json::JSON(subtitle), window_, queue_);

  auto ptr = shared_from_this();
  subtitle_.Up(event::Bind(&ScriptImpl::Event, ptr, "up"));
  subtitle_.Down(event::Bind(&ScriptImpl::Event, ptr, "down"));
  subtitle_.Left(event::Bind(&ScriptImpl::Event, ptr, "left"));
  subtitle_.Right(event::Bind(&ScriptImpl::Event, ptr, "right"));

  lua_.Add(event::Bind(&ScriptImpl::Command, ptr), "command");

  lua_.Load(story);
  lua_.Get("begin");
  lua_.Call(0, 0);
}

void ScriptImpl::Command(void)
{
  std::string command;
  lua_.PopFront(command);
  if(command == "item")
  {
    lua_.PopFront(command);
    if(command == "state")
    {
      std::string name;
      std::string state;
      lua_.PopFront(name);
      lua_.PopFront(state);
      auto range = stage_->items_.equal_range(state);
      for(auto item = range.first; item != range.second; ++item)
      {
        item->second.State(state);
      }
    }
    else if(command == "new")
    {
      std::string name;
      std::string json;
      lua_.PopFront(name);
      lua_.PopFront(json);
      Item item(json::JSON(json), window_, stage_->scene_, queue_, ccollision_, stage_->world_);
      auto ptr = shared_from_this();
      item.Hysteresis(event::Bind(&ScriptImpl::Event, ptr, name), event::Bind(&ScriptImpl::Event, ptr, "exit"));
      if(!paused_)
      {
        item.Resume();
      }
      stage_->items_.emplace(name, item);
    }
    else if(command == "delete")
    {
      std::string name;
      lua_.PopFront(name);
      stage_->items_.erase(name);
    }
    else
    {
      BOOST_THROW_EXCEPTION(exception::Exception());
    }
  }
  else if(command == "box")
  {
    lua_.PopFront(command);
    if(command == "new")
    {
      std::string name;
      std::string json;
      lua_.PopFront(name);
      lua_.PopFront(json);
      Box box(json::JSON(json), window_, stage_->scene_, queue_, dcollision_, stage_->world_);
      if(!paused_)
      {
        box.Resume();
      }
      stage_->boxes_.emplace(name, box);
    }
    else if(command == "delete")
    {
      std::string name;
      lua_.PopFront(name);
      stage_->items_.erase(name);
    }
    else
    {
      BOOST_THROW_EXCEPTION(exception::Exception());
    }
  }
  else if(command == "stage")
  {
    lua_.PopFront(command);
    if(command == "change")
    {
      std::string name;
      lua_.PopFront(name);
      bool paused = paused_;
      Pause();
      stage_ = stages_.at(name);
      if(!paused)
      {
        Resume();
      }
    }
    else if(command == "new")
    {
      std::string name;
      lua_.PopFront(name);
      bool paused = paused_;
      Pause();
      stage_ = stages_.emplace(name, std::make_shared<Stage>()).first->second;
      if(!paused)
      {
        Resume();
      }
    }
    else if(command == "delete")
    {
      std::string name;
      lua_.PopFront(name);
      stages_.erase(name);
    }
    else
    {
      BOOST_THROW_EXCEPTION(exception::Exception());
    }
  }
  else if(command == "terrain")
  {
    lua_.PopFront(command);
    if(command == "new")
    {
      std::string name;
      std::string json;
      lua_.PopFront(name);
      lua_.PopFront(json);
      stage_->terrain_.emplace(name, Terrain(json::JSON(json), window_, stage_->scene_, dcollision_, stage_->world_));
    }
    else if(command == "delete")
    {
      std::string name;
      lua_.PopFront(name);
      stage_->terrain_.erase(name);
    }
    else if(command == "modulate")
    {
      std::string name;
      float r, g, b;
      lua_.PopFront(name);
      lua_.PopFront(r);
      lua_.PopFront(g);
      lua_.PopFront(b);
      auto iter = stage_->terrain_.find(name);
      if(iter != stage_->terrain_.end())
      {
        iter->second.Modulation(r, g, b);
      }
      else
      {
        BOOST_THROW_EXCEPTION(exception::Exception());
      }
    }
    else
    {
      BOOST_THROW_EXCEPTION(exception::Exception());
    }
  }
  else if(command == "scene")
  {
    lua_.PopFront(command);
    if(command == "new")
    {
      std::string json;
      lua_.PopFront(json);
      stage_->scene_ = game::Scene(json::JSON(json), window_);
    }
    else if(command == "modulate")
    {
      float r, g, b;
      lua_.PopFront(r);
      lua_.PopFront(g);
      lua_.PopFront(b);
      stage_->scene_.Modulation(r, g, b);
    }
    else
    {
      BOOST_THROW_EXCEPTION(exception::Exception());
    }
  }
  else if(command == "world")
  {
    lua_.PopFront(command);
    if(command == "new")
    {
      std::string json;
      lua_.PopFront(json);
      dynamics::World world(json::JSON(json), collision_, queue_);
      world.End(event::Bind(&ScriptImpl::View, shared_from_this()));
      if(!paused_)
      {
        world.Resume();
      }
      stage_->world_ = world;
    }
    else if(command == "ambient")
    {
      float r, g, b;
      lua_.PopFront(r);
      lua_.PopFront(g);
      lua_.PopFront(b);
      stage_->world_.Ambient(r, g, b);
    }
    else
    {
      BOOST_THROW_EXCEPTION(exception::Exception());
    }
  }
  else if(command == "hero")
  {
    lua_.PopFront(command);
    if(command == "new")
    {
      std::string json;
      lua_.PopFront(json);
      Hero hero(json::JSON(json), window_, stage_->scene_, dcollision_, ccollision_, queue_, stage_->world_);
      if(!paused_)
      {
        hero.Resume();
      }
      stage_->hero_ = hero;
    }
    else if(command == "state")
    {
      std::string state;
      lua_.PopFront(state);
      stage_->hero_.State(state);
    }
    else if(command == "position")
    {
      Position position;
      lua_.PopFront(position.first);
      lua_.PopFront(position.second);
      stage_->hero_.Position(position);
    }
    else
    {
      BOOST_THROW_EXCEPTION(exception::Exception());
    }
  }
  else if(command == "music")
  {
    lua_.PopFront(command);
    if(command == "new")
    {
      std::string json;
      lua_.PopFront(json);
      audio::Music music(json);
      music.Volume(0.01);
      music.Play();
      if(paused_)
      {
        music.Pause();
      }
      stage_->music_ = music;
    }
    else
    {
      BOOST_THROW_EXCEPTION(exception::Exception());
    }
  }
  else
  {
    BOOST_THROW_EXCEPTION(exception::Exception());
  }
}

void ScriptImpl::Event(std::string const& event)
{
  lua_.Get("event");
  lua_.Push(event);
  lua_.Call(1, 11);

  std::string text;
  lua_.PopFront(text);
  subtitle_.Text(text);

  std::string up;
  lua_.PopFront(up);
  std::string down;
  lua_.PopFront(down);
  std::string left;
  lua_.PopFront(left);
  std::string right;
  lua_.PopFront(right);
  subtitle_.Choice(up, down, left, right);

  double timer;
  lua_.PopFront(timer);
  if(timer > 0.)
  {
    timer_ = event::Timer(timer, 0);
    queue_.Add(event::Bind(&event::Timer::operator(), timer_));
    timer_.Add(event::Bind(&ScriptImpl::Event, shared_from_this(), "timer"));
    timer_.Resume();
  }
  else
  {
    timer_ = event::Timer();
  }

  lua_.PopFront(focus_.first);
  lua_.PopFront(focus_.second);
  lua_.PopFront(zoom_);
  lua_.PopFront(subject_focus_);
  lua_.PopFront(subject_hero_);
}

void ScriptImpl::View(void)
{
  game::Position p;
  if(subject_hero_)
  {
    p = stage_->hero_.Position();
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
    p = stage_->hero_.Position();
    p.second -= 200.f;
  }
  window_.View(p.first, p.second, zoom_);
}

void ScriptImpl::Pause(void)
{
  if(!paused_)
  {
    paused_ = true;
    stage_->music_.Pause();
    for(auto& item : stage_->items_)
    {
      item.second.Pause();
    }
    for(auto& box : stage_->boxes_)
    {
      box.second.Pause();
    }
    stage_->hero_.Pause();
    if(timer_)
    {
      timer_.Pause();
    }
    subtitle_.Pause();
    stage_->world_.Pause();
  }
}

void ScriptImpl::Resume(void)
{
  if(paused_)
  {
    paused_ = false;
    stage_->music_.Resume();
    for(auto& item : stage_->items_)
    {
      item.second.Resume();
    }
    for(auto& box : stage_->boxes_)
    {
      box.second.Resume();
    }
    stage_->hero_.Resume();
    if(timer_)
    {
      timer_.Resume();
    }
    subtitle_.Resume();
    stage_->world_.Resume();
  }
}

void ScriptImpl::Render(void)
{
  stage_->scene_.Render();
  subtitle_.Render();
}

void ScriptImpl::ChoiceUp(void)
{
  subtitle_.Up();
}

void ScriptImpl::ChoiceDown(void)
{
  subtitle_.Down();
}

void ScriptImpl::ChoiceLeft(void)
{
  subtitle_.Left();
}

void ScriptImpl::ChoiceRight(void)
{
  subtitle_.Right();
}

void ScriptImpl::Up(void)
{
  stage_->hero_.Up();
}

void ScriptImpl::Down(void)
{
  stage_->hero_.Down();
}

void ScriptImpl::Left(void)
{
  stage_->hero_.Left();
}

void ScriptImpl::Right(void)
{
  stage_->hero_.Right();
}

void Script::Pause(void)
{
  impl_->Pause();
}

void Script::Resume(void)
{
  impl_->Resume();
}

void Script::Render(void)
{
  impl_->Render();
}

void Script::ChoiceUp(void)
{
  impl_->ChoiceUp();
}

void Script::ChoiceDown(void)
{
  impl_->ChoiceDown();
}

void Script::ChoiceLeft(void)
{
  impl_->ChoiceLeft();
}

void Script::ChoiceRight(void)
{
  impl_->ChoiceRight();
}

void Script::Up(void)
{
  impl_->Up();
}

void Script::Down(void)
{
  impl_->Down();
}

void Script::Left(void)
{
  impl_->Left();
}

void Script::Right(void)
{
  impl_->Right();
}

Script::operator bool(void) const
{
  return bool(impl_);
}

Script::Script(json::JSON const& json, display::Window& window, event::Queue& queue)
{
  impl_ = std::make_shared<ScriptImpl>(window, queue);
  impl_->Init(json);
}
}