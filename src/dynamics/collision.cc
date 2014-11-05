#include "collision.h"
#include <map>
#include "switch.h"
namespace collision
{
namespace
{
typedef std::map<dynamics::Body::WeakPtr, std::map<dynamics::Body::WeakPtr, event::Switch>> CollisionMap;
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

void Collision::Impl::Unlink(dynamics::Body::WeakPtr const& a)
{
  auto iter_a = collisions_.find(a);
  if(iter_a != collisions_.end())
  {
    for(auto element : iter_a->second)
    {
      collisions_[element.first].erase(a);
    }
    collisions_.erase(iter_a);
  }
}

void Collision::Impl::Unlink(dynamics::Body::WeakPtr const& a, dynamics::Body::WeakPtr const& b)
{
  auto iter_a = collisions_.find(a);
  if(iter_a != collisions_.end())
  {
    auto iter_b = iter_a->second.find(b);
    if(iter_b != iter_a->second.end())
    {
      iter_a->second.erase(iter_b);
    }
    if(iter_a->second.empty())
    {
      collisions_.erase(iter_a);
    }
  }
}

void Collision::Impl::Begin(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c)
{
  if(Link(a, b))
  {
    collisions_[a][b].first.Add(c);
  }
}

void Collision::Impl::End(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c)
{
  if(Link(a, b))
  {
    collisions_[a][b].second.Add(c);
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
  impl_->Unlink(b, a);
}
}