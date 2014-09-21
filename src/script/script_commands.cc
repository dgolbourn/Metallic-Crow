#include "script_impl.h"
#include "exception.h"
#include "bind.h"
namespace game
{
void Script::Impl::Command(void)
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
      item.Hysteresis(event::Bind(&Script::Impl::Event, shared_from_this(), name), event::Bind(&Script::Impl::Event, shared_from_this(), "exit"));
      if(!stage_->paused_)
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
      if(!stage_->paused_)
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
      stage_ = stages_.find(name)->second;
    }
    else if(command == "new")
    {
      std::string name;
      lua_.PopFront(name);
      stage_ = stages_.emplace(name, StagePtr(new Stage)).first->second;
      stage_->paused_ = true;
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
      world.End(event::Bind(&Impl::View, shared_from_this()));
      if(!stage_->paused_)
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
      Actor hero(json::JSON(json), window_, stage_->scene_, dcollision_, ccollision_, queue_, stage_->world_);
      if(!stage_->paused_)
      {
        hero.Resume();
      }
      stage_->hero_ = hero;
    }
    else if(command == "body")
    {
      std::string expression;
      lua_.PopFront(expression);
      stage_->hero_.Body(expression);
    }
    else if(command == "eyes")
    {
      std::string expression;
      lua_.PopFront(expression);
      stage_->hero_.Eyes(expression);
    }
    else if(command == "mouth")
    {
      std::string expression;
      lua_.PopFront(expression);
      stage_->hero_.Mouth(expression);
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
      if(stage_->paused_)
      {
        music.Pause();
      }
      track_ = music;
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
}