#include "world.h"
#include "world_impl.h"
#include "body_impl.h"
#include "units.h"
#include "bind.h"
#include <boost/iterator/iterator_facade.hpp>
namespace dynamics
{
bool WorldImpl::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
  return collision_.Check(BodyImpl::MakeBody(fixtureA->GetBody()), BodyImpl::MakeBody(fixtureB->GetBody()));
}

void WorldImpl::BeginContact(b2Contact* contact)
{
  return collision_(BodyImpl::MakeBody(contact->GetFixtureA()->GetBody()), BodyImpl::MakeBody(contact->GetFixtureB()->GetBody()), true);
}

void WorldImpl::EndContact(b2Contact* contact)
{
  return collision_(BodyImpl::MakeBody(contact->GetFixtureA()->GetBody()), BodyImpl::MakeBody(contact->GetFixtureB()->GetBody()), false);
}

class BodyImpl::Iterator : public boost::iterator_facade<Iterator, BodyImpl, boost::forward_traversal_tag>
{ 
public:
  Iterator(void) : impl_(nullptr) 
  {
  }

  explicit Iterator(BodyImpl* impl) : impl_(impl) 
  {
  }

private:
  friend class boost::iterator_core_access;

  void increment(void)
  {
    if(impl_)
    {
      if(b2Body* body = impl_->body_)
      {
        while((body = body->GetNext()) && (body->GetType() == b2_staticBody))
        {
        }
        if(body)
        {
          impl_ = (BodyImpl*)body->GetUserData();
        }
        else
        {
          impl_ = nullptr;
        }
      }
    }
  }

  bool equal(Iterator const& other) const
  {
    return this->impl_ == other.impl_;
  }

  BodyImpl& dereference(void) const 
  { 
    return *impl_; 
  }

  BodyImpl* impl_;
};

class BodyImpl::Range
{
private:
  b2World& world_;

public:
  Range(b2World& world) : world_(world) 
  {
  };

  Iterator begin(void)
  {
    BodyImpl* impl = nullptr;
    b2Body* body = world_.GetBodyList();
    while(body && (body->GetType() == b2_staticBody))
    {
      body = body->GetNext();
    }
    if(body)
    {
      impl = (BodyImpl*)body->GetUserData();
    }
    return Iterator(impl);
  }

  Iterator end(void)
  {
    return Iterator();
  }
};

void WorldImpl::Update(void)
{
  if(!paused_)
  {
    Clock::duration elapsed = Clock::now() - tick_;

    while(elapsed >= interval_)
    {
      elapsed -= interval_;
      tick_ += interval_;

      begin_();

      for(auto& body : BodyImpl::Range(world_))
      {
        body.Begin();
      }

      world_.Step(dt_, velocity_iterations_, position_iterations_);

      for(auto& body : BodyImpl::Range(world_))
      {
         body.End(dt_);
      }
    }

    static const float32 scale = float32(Clock::period::num) / float32(Clock::period::den);
    float32 ds = (float32)elapsed.count() * scale / dt_;

    for(auto& body : BodyImpl::Range(world_))
    {
      body.Update(ds);
    }

    end_();
  }
}

void WorldImpl::Begin(event::Command const& command)
{
  begin_.Add(command);
}

void WorldImpl::End(event::Command const& command)
{
  end_.Add(command);
}

WorldImpl::WorldImpl(json::JSON const& json, game::Collision& collision) : world_(b2Vec2(0, 0)), velocity_iterations_(8), position_iterations_(3), collision_(collision), paused_(true)
{
  double x;
  double y;

  json.Unpack("{s[ff]}",
    "gravity", &x, &y);

  double interval = 1. / 60.;

  dt_ = float32(interval);
  static const double scale = double(Clock::period::den) / double(Clock::period::num);
  interval *= scale;
  interval_ = Clock::duration(Clock::rep(interval));
  remaining_ = interval_;

  world_.SetGravity(b2Vec2(Metres(float32(x)),Metres(float32(y))));
  world_.SetAutoClearForces(true);
  world_.SetContactListener(this);
  world_.SetContactFilter(this);
}

void WorldImpl::Init(event::Queue& queue)
{
  queue.Add(event::Bind(&WorldImpl::Update, shared_from_this()));
}

void WorldImpl::Pause(void)
{
  if(!paused_)
  {
    remaining_ = interval_ + tick_ - Clock::now();
    paused_ = true;
  }
}

void WorldImpl::Resume(void)
{
  if(paused_)
  {
    tick_ = Clock::now() - interval_ + remaining_;
    paused_ = false;
  }
}

World::World(json::JSON const& json, game::Collision& collision, event::Queue& queue)
{
  impl_ = std::make_shared<WorldImpl>(json, collision);
  impl_->Init(queue);
}

void World::Begin(event::Command const& command)
{
  impl_->Begin(command);
}

void World::End(event::Command const& command)
{
  impl_->End(command);
}

World::operator bool(void) const
{
  return bool(impl_);
}

void World::Pause(void)
{
  impl_->Pause();
}

void World::Resume(void)
{
  impl_->Resume();
}
}