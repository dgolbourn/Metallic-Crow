#include "collision.h"
#include <map>
#include "switch.h"
namespace game
{
typedef std::pair<dynamics::Body::WeakPtr, dynamics::Body::WeakPtr> BodyPair;
typedef std::map<BodyPair, event::Switch> CollisionMap;

class CollisionImpl
{
public:
  CollisionImpl(event::Queue& queue);
  void Add(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c, bool start);
  bool Check(dynamics::Body const& a, dynamics::Body const& b) const;
  void Notify(dynamics::Body const& a, dynamics::Body const& b, bool start);
  void Clear(void);
  CollisionMap collisions_;
  event::Queue queue_;
};

CollisionImpl::CollisionImpl(event::Queue& queue) : queue_(queue)
{
}

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

void CollisionImpl::Add(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c, bool start)
{
  if(start)
  {
    collisions_[MakePair(a, b)].first.Add(c);
  }
  else
  {
    collisions_[MakePair(a, b)].second.Add(c);
  }
}

bool CollisionImpl::Check(dynamics::Body const& a, dynamics::Body const& b) const
{
  return collisions_.find(MakePair(a, b)) != collisions_.end();
}

static bool Empty(event::Switch const& s)
{
  return !(bool(s.first) || bool(s.second));
}

void CollisionImpl::Notify(dynamics::Body const& a, dynamics::Body const& b, bool start)
{
  auto iter = collisions_.find(MakePair(a, b));
  if(iter != collisions_.end())
  {
    if(start)
    {
      iter->second.first(queue_);
    }
    else
    {
      iter->second.second(queue_);
    }
    if(Empty(iter->second))
    {
      iter = collisions_.erase(iter);
    }
  }
}

void CollisionImpl::Clear(void)
{
  collisions_.clear();
}

Collision::Collision(event::Queue& queue)
{
  impl_ = std::make_shared<CollisionImpl>(queue);
}

void Collision::Add(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c, bool start)
{
  impl_->Add(a, b, c, start);
}

bool Collision::Check(dynamics::Body const& a, dynamics::Body const& b) const
{
  return impl_->Check(a, b);
}

void Collision::operator()(dynamics::Body const& a, dynamics::Body const& b, bool start)
{
  impl_->Notify(a, b, start);
}

void Collision::Clear(void)
{
  impl_->Clear();
}
}