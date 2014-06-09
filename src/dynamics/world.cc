#include "world.h"
#include "world_impl.h"
#include "body_impl.h"
#include "units.h"
#include "bind.h"
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

void WorldImpl::Step(void)
{
  begin_();
  world_.Step(interval_, velocity_iterations_, position_iterations_);
  end_();
}

void WorldImpl::Begin(event::Command const& command)
{
  begin_.Add(command);
}

void WorldImpl::End(event::Command const& command)
{
  end_.Add(command);
}

WorldImpl::WorldImpl(json::JSON const& json, game::Collision& collision, double& interval) : world_(b2Vec2(0, 0)), velocity_iterations_(8), position_iterations_(3), collision_(collision)
{
  double x;
  double y;

  json.Unpack("{s[ff]}",
    "gravity", &x, &y);

  interval = 1. / 60.;
  interval_ = float32(interval);

  world_.SetGravity(b2Vec2(Metres(float32(x)),Metres(float32(y))));
  world_.SetAutoClearForces(true);
  world_.SetContactListener(this);
  world_.SetContactFilter(this);
}

void WorldImpl::Init(event::Queue& queue, double interval)
{
  timer_ = event::Timer(interval, -1);
  timer_.Add(event::Bind(&WorldImpl::Step, shared_from_this()));
  queue.Add(event::Bind(&event::Timer::operator(), timer_));
}

void WorldImpl::Pause(void)
{
  timer_.Pause();
}

void WorldImpl::Resume(void)
{
  timer_.Resume();
}

World::World(json::JSON const& json, game::Collision& collision, event::Queue& queue)
{
  double interval;
  impl_ = std::make_shared<WorldImpl>(json, collision, interval);
  impl_->Init(queue, interval);
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