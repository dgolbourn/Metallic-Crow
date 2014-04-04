#include "world.h"
#include "world_impl.h"
#include "body_impl.h"
namespace dynamics
{
bool WorldImpl::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
  return collision_.Check(BodyImpl::MakeBody(fixtureA->GetBody()), BodyImpl::MakeBody(fixtureB->GetBody()));
}

void WorldImpl::BeginContact(b2Contact* contact)
{
  return collision_.Signal(BodyImpl::MakeBody(contact->GetFixtureA()->GetBody()), BodyImpl::MakeBody(contact->GetFixtureB()->GetBody()));
}

void WorldImpl::Step(void)
{
  world_.Step(t_, velocity_iterations_, position_iterations_);
  signal_();
}

void WorldImpl::Add(event::Command const& command)
{
  signal_.Add(command);
}

WorldImpl::WorldImpl(json::JSON const& json, game::Collision& collision) : world_(b2Vec2(0, 0)), t_(1.f / 60.f), velocity_iterations_(8), position_iterations_(3), collision_(collision)
{
  double x;
  double y;

  json.Unpack("{s[ff]}",
    "gravity", &x, &y);

  world_.SetGravity(b2Vec2(float32(x),float32(y)));
  world_.SetAutoClearForces(true);
  world_.SetContactListener(this);
  world_.SetContactFilter(this);
}

World::World(json::JSON const& json, game::Collision& collision)
{
  impl_ = std::make_shared<WorldImpl>(json, collision);
}

void World::Step(void)
{
  impl_->Step();
}

void World::Add(event::Command const& command)
{
  impl_->Add(command);
}

World::operator bool(void) const
{
  return bool(impl_);
}
}