#include "collision.h"
#include <map>
#include "signal.h"
namespace
{
typedef std::map<dynamics::Body::WeakPtr, std::map<dynamics::Body::WeakPtr, std::pair<event::Signal, event::Signal>>> CollisionMap;

auto Remove(CollisionMap& collisions, dynamics::Body::WeakPtr const& a, dynamics::Body::WeakPtr const& b) -> void
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

namespace collision
{
class Collision::Impl
{
public:
  Impl(event::Queue& queue);
  auto Begin(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c) -> void;
  auto End(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c) -> void;
  auto Check(dynamics::Body const& a, dynamics::Body const& b) const -> bool;
  auto Begin(dynamics::Body const& a, dynamics::Body const& b) -> void;
  auto End(dynamics::Body const& a, dynamics::Body const& b) -> void;
  auto Link(dynamics::Body const& a, dynamics::Body const& b) -> bool;
  auto Unlink(dynamics::Body::WeakPtr const& a) -> void;
  auto Unlink(dynamics::Body::WeakPtr const& a, dynamics::Body::WeakPtr const& b) -> void;
  CollisionMap collisions_;
  event::Queue queue_;
};

Collision::Impl::Impl(event::Queue& queue) : queue_(queue)
{
}

auto Collision::Impl::Unlink(dynamics::Body::WeakPtr const& body_a) -> void
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

auto Collision::Impl::Unlink(dynamics::Body::WeakPtr const& a, dynamics::Body::WeakPtr const& b) -> void
{
  Remove(collisions_, a, b);
  Remove(collisions_, b, a);
}

auto Collision::Impl::Begin(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c) -> void
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

auto Collision::Impl::End(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c) -> void
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

auto Collision::Impl::Check(dynamics::Body const& a, dynamics::Body const& b) const -> bool
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

auto Collision::Impl::Begin(dynamics::Body const& a, dynamics::Body const& b) -> void
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

auto Collision::Impl::End(dynamics::Body const& a, dynamics::Body const& b) -> void
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

auto Collision::Impl::Link(dynamics::Body const& a, dynamics::Body const& b) -> bool
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

auto Collision::Begin(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c) -> void
{
  impl_->Begin(a, b, c);
}

auto Collision::End(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c) -> void
{
  impl_->End(a, b, c);
}

auto Collision::Link(dynamics::Body const& a, dynamics::Body const& b) -> void
{
  impl_->Link(a, b);
}

auto Collision::Check(dynamics::Body const& a, dynamics::Body const& b) const -> bool
{
  return impl_->Check(a, b);
}

auto Collision::Begin(dynamics::Body const& a, dynamics::Body const& b) -> void
{
  impl_->Begin(a, b);
}

auto Collision::End(dynamics::Body const& a, dynamics::Body const& b) -> void
{
  impl_->End(a, b);
}

auto Collision::Unlink(dynamics::Body::WeakPtr const& a) -> void
{
  impl_->Unlink(a);
}

auto Collision::Unlink(dynamics::Body::WeakPtr const& a, dynamics::Body::WeakPtr const& b) -> void
{
  impl_->Unlink(a, b);
}
}