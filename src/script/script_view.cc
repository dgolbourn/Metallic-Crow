#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::ViewInit()
{
  lua_.Add(function::Bind(&Impl::ViewAddActor, shared_from_this()), "view_add_actor", 0);
  lua_.Add(function::Bind(&Impl::ViewActor, shared_from_this()), "view_actor", 0);
  lua_.Add(function::Bind(&Impl::ViewPoint, shared_from_this()), "view_point", 0);
  lua_.Add(function::Bind(&Impl::ViewZoom, shared_from_this()), "view_zoom", 0);
}

void Script::Impl::ViewAddActor()
{
  std::string name;
  lua_.PopFront(name);
  auto actor = stage_->actors_.find(name);
  if(actor != stage_->actors_.end())
  {
    stage_->subjects_.push_back(actor->second);
  }
}

void Script::Impl::ViewActor()
{
  std::string name;
  lua_.PopFront(name);
  auto actor = stage_->actors_.find(name);
  if(actor != stage_->actors_.end())
  {
    stage_->subjects_.clear();
    stage_->subjects_.push_back(actor->second);
  }
}

void Script::Impl::ViewPoint()
{
  float x;
  float y;
  lua_.PopFront(x);
  lua_.PopFront(y);
  stage_->subject_ = Position(x, y);
  stage_->subjects_.clear();
}

void Script::Impl::ViewZoom()
{
  float z;
  lua_.PopFront(z);
  stage_->zoom_ = z;
}

void Script::Impl::View(dynamics::World::WeakPtr world)
{
  if(world.Lock() == stage_->world_)
  {
    game::Position view(0.f, 0.f);
    int count = 0;

    if(stage_->subject_)
    {
      view.first += stage_->subject_->first;
      view.second += stage_->subject_->second;
      ++count;
    }

    for(auto iter = stage_->subjects_.begin(); iter != stage_->subjects_.end();)
    {
      if(Actor subject = iter->Lock())
      {
        game::Position position = subject.Position();
        view.first += position.first;
        view.second += position.second;
        ++count;
        ++iter;
      }
      else
      {
        iter = stage_->subjects_.erase(iter);
      }
    }

    window_.View(view.first / count, view.second / count, stage_->zoom_);
  }
}
}