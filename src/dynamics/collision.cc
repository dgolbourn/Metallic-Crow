#include "collision.h"
#include <map>
#include "signal.h"
namespace collision
{
namespace
{
typedef std::map<dynamics::Body::WeakPtr, std::map<dynamics::Body::WeakPtr, std::pair<event::Signal, event::Signal>>> CollisionMap;

void Remove(CollisionMap& collisions, dynamics::Body::WeakPtr const& a, dynamics::Body::WeakPtr const& b)
{
  auto iter_a = collisions.find(a);
  if(iter_a != collisions.end())
  {
    auto iter_b = iter_a->second.find(b);
    if(iter_b != iter_a->second.end())
    {
      iter_a->second.erase(iter_b);
      if(iter_a->second.empty())
      {
        collisions.erase(iter_a);
      }
    }
  }
}
}

class Collision::Impl
{
public:
  Impl(event::Queue& queue);

  void Begin(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c);
  void End(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c);
  bool Check(dynamics::Body const& a, dynamics::Body const& b) const;
  void Begin(dynamics::Body const& a, dynamics::Body const& b);
  void End(dynamics::Body const& a, dynamics::Body const& b);

  bool Link(dynamics::Body const& a, dynamics::Body const& b);
  void Unlink(dynamics::Body::WeakPtr const& a);
  void Unlink(dynamics::Body::WeakPtr const& a, dynamics::Body::WeakPtr const& b);

  CollisionMap collisions_;
  event::Queue queue_;
};

Collision::Impl::Impl(event::Queue& queue) : queue_(queue)
{
}

void Collision::Impl::Unlink(dynamics::Body::WeakPtr const& body_a)
{
  auto iter_a = collisions_.find(body_a);
  if(iter_a != collisions_.end())
  {
    std::list<CollisionMap::const_iterator> erase;
    erase.push_back(iter_a);

    for(auto body_b : iter_a->second)
    {
      auto iter_b = collisions_.find(body_b.first);
      if(iter_b != collisions_.end())
      {
        iter_b->second.erase(body_a);
        if(iter_b->second.empty())
        {
          erase.push_back(iter_b);
        }
      }
    }

    for(auto body : erase)
    {
      collisions_.erase(body);
    }
  }
}

void Collision::Impl::Unlink(dynamics::Body::WeakPtr const& a, dynamics::Body::WeakPtr const& b)
{
  Remove(collisions_, a, b);
  Remove(collisions_, b, a);
}

void Collision::Impl::Begin(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c)
{
  auto iter_a = collisions_.find(a);
  if(iter_a != collisions_.end())
  {
    auto iter_b = iter_a->second.find(b);
    if(iter_b != iter_a->second.end())
    {
      iter_b->second.first.Add(c);
    }
  }
}

void Collision::Impl::End(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c)
{
  auto iter_a = collisions_.find(a);
  if(iter_a != collisions_.end())
  {
    auto iter_b = iter_a->second.find(b);
    if(iter_b != iter_a->second.end())
    {
      iter_b->second.second.Add(c);
    }
  }
}

bool Collision::Impl::Check(dynamics::Body const& a, dynamics::Body const& b) const
{
  bool collision = false;
  auto iter_a = collisions_.find(a);
  if(iter_a != collisions_.end())
  {
    auto iter_b = iter_a->second.find(b);
    if(iter_b != iter_a->second.end())
    {
      collision = true;
    }
  }
  return collision;
}

void Collision::Impl::Begin(dynamics::Body const& a, dynamics::Body const& b)
{
  auto iter_a = collisions_.find(a);
  if(iter_a != collisions_.end())
  {
    auto iter_b = iter_a->second.find(b);
    if(iter_b != iter_a->second.end())
    {
      iter_b->second.first(queue_);
    }
  }
}

void Collision::Impl::End(dynamics::Body const& a, dynamics::Body const& b)
{
  auto iter_a = collisions_.find(a);
  if(iter_a != collisions_.end())
  {
    auto iter_b = iter_a->second.find(b);
    if(iter_b != iter_a->second.end())
    {
      iter_b->second.second(queue_);
    }
  }
}

bool Collision::Impl::Link(dynamics::Body const& a, dynamics::Body const& b)
{
  bool link = false;
  if((a < b) || (b < a))
  {
    collisions_[b][a] = collisions_[a][b];
    link = true;
  }
  return link;
}

Collision::Collision(event::Queue& queue) : impl_(std::make_shared<Impl>(queue))
{
}

void Collision::Begin(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c)
{
  impl_->Begin(a, b, c);
}

void Collision::End(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c)
{
  impl_->End(a, b, c);
}

void Collision::Link(dynamics::Body const& a, dynamics::Body const& b)
{
  impl_->Link(a, b);
}

bool Collision::Check(dynamics::Body const& a, dynamics::Body const& b) const
{
  return impl_->Check(a, b);
}

void Collision::Begin(dynamics::Body const& a, dynamics::Body const& b)
{
  impl_->Begin(a, b);
}

void Collision::End(dynamics::Body const& a, dynamics::Body const& b)
{
  impl_->End(a, b);
}

void Collision::Unlink(dynamics::Body::WeakPtr const& a)
{
  impl_->Unlink(a);
}

void Collision::Unlink(dynamics::Body::WeakPtr const& a, dynamics::Body::WeakPtr const& b)
{
  impl_->Unlink(a, b);
}
}