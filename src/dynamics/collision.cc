#include "collision.h"
#include <list>
#include <map>
namespace game
{
typedef std::pair<dynamics::Body::WeakPtr, dynamics::Body::WeakPtr> BodyPair;
typedef std::list<event::Command> CommandList;
typedef std::map<BodyPair, CommandList> CollisionMap;

class CollisionImpl
{
public:
  void Add(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c);
  bool Check(dynamics::Body const& a, dynamics::Body const& b) const;
  void Signal(dynamics::Body const& a, dynamics::Body const& b);
  void Clear(void);
  CollisionMap collisions_;
};

static BodyPair MakePair(dynamics::Body const& a, dynamics::Body const& b)
{
  BodyPair body_pair;
  if(b < a)
  {
    body_pair = BodyPair(b, a);
  }
  else
  {
    body_pair = BodyPair(a, b);
  }
  return body_pair;
}

void CollisionImpl::Add(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c)
{
  collisions_[MakePair(a, b)].push_back(c);
}

bool CollisionImpl::Check(dynamics::Body const& a, dynamics::Body const& b) const
{
  return collisions_.find(MakePair(a, b)) != collisions_.end();
}

void CollisionImpl::Signal(dynamics::Body const& a, dynamics::Body const& b)
{
  auto iter = collisions_.find(MakePair(a, b));
  if(iter != collisions_.end())
  {
    for(auto command_iter = iter->second.begin(); command_iter != iter->second.end();)
    {
      if((*command_iter)())
      {
        ++command_iter;
      }
      else
      {
        command_iter = iter->second.erase(command_iter);
      }
    }
    if(iter->second.empty())
    {
      iter = collisions_.erase(iter);
    }
  }
}

void CollisionImpl::Clear(void)
{
  collisions_.clear();
}

Collision::Collision(void)
{
  impl_ = std::make_shared<CollisionImpl>();
}

void Collision::Add(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c)
{
  impl_->Add(a, b, c);
}

bool Collision::Check(dynamics::Body const& a, dynamics::Body const& b) const
{
  return impl_->Check(a, b);
}

void Collision::Signal(dynamics::Body const& a, dynamics::Body const& b)
{
  impl_->Signal(a, b);
}

void Collision::Clear(void)
{
  impl_->Clear();
}
}