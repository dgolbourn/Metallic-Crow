#include "world.h"
#include "world_impl.h"
#include "body_impl.h"
#include "bind.h"
#include "body_impl_iterator.h"
namespace dynamics
{
void WorldImpl::SayGoodbye(b2Joint* joint)
{
  joint->SetUserData(nullptr);
}

void WorldImpl::SayGoodbye(b2Fixture*)
{
}

bool WorldImpl::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
  return collision_.Check(BodyImpl::MakeBody(fixtureA->GetBody()), BodyImpl::MakeBody(fixtureB->GetBody()));
}

static bool ToggleContact(b2Contact* contact, BodyImplCount& contacts, bool begin)
{
  BodyImpl* body_a = (BodyImpl*)contact->GetFixtureA()->GetBody()->GetUserData();
  BodyImpl* body_b = (BodyImpl*)contact->GetFixtureB()->GetBody()->GetUserData();
  BodyImplPair pair = Make(body_a, body_b);
  bool trigger = false;
  auto iter = contacts.find(pair);
  if(begin)
  {
    if(iter == contacts.end())
    {
      contacts.emplace(pair, 1);
      trigger = true;
    }
    else
    {
      ++iter->second;
    }
  }
  else
  {
    if(iter == contacts.end())
    {
      trigger = true;
    }
    else
    {
      --iter->second;
      if(iter->second <= 0)
      {
        contacts.erase(iter);
        trigger = true;
      }
    }
  }
  return trigger;
}

void WorldImpl::BeginContact(b2Contact* contact)
{
  if(ToggleContact(contact, contact_, true))
  {
    collision_.Begin(BodyImpl::MakeBody(contact->GetFixtureA()->GetBody()), BodyImpl::MakeBody(contact->GetFixtureB()->GetBody()));
  }
}

void WorldImpl::EndContact(b2Contact* contact)
{
  if(ToggleContact(contact, contact_, false))
  {
    collision_.End(BodyImpl::MakeBody(contact->GetFixtureA()->GetBody()), BodyImpl::MakeBody(contact->GetFixtureB()->GetBody()));
  }
}

void WorldImpl::Update(void)
{
  if(!paused_)
  {
    Clock::duration elapsed = Clock::now() - tick_;
    
    if(elapsed >= interval_)
    {
      begin_();

      do
      {
        for(auto& body : world_)
        {
          body.Begin();
        }

        world_.Step(dt_, 8, 3);

        for(auto& body : world_)
        {
          body.End(dt_);
        }

        elapsed -= interval_;
        tick_ += interval_;
      } 
      while(elapsed >= interval_);

      world_.ClearForces();

      Light();
    }

    static const float32 scale = float32(Clock::period::num) / float32(Clock::period::den);
    float32 ds = (float32)elapsed.count() * scale / dt_;

    for(auto& body : world_)
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

WorldImpl::WorldImpl(json::JSON const& json, collision::Collision& collision) : world_(b2Vec2(0, 0)), collision_(collision), paused_(true)
{
  double x, y, r, g, b, rate, scale;

  json.Unpack("{s[ff]s[fff]sfsf}",
    "gravity", &x, &y,
    "ambient", &r, &g, &b,
    "frame rate", &rate,
    "scale", &scale);

  scale_ = float32(scale);

  ambient_.Set(float32(r), float32(g), float32(b));

  f_ = float32(rate);

  double interval = 1. / rate;

  dt_ = float32(interval);
  interval *= double(Clock::period::den) / double(Clock::period::num);
  interval_ = Clock::duration(Clock::rep(interval));
  remaining_ = interval_;

  world_.SetGravity(b2Vec2(Metres(x), Metres(y)));
  world_.SetAutoClearForces(false);
  world_.SetContactListener(this);
  world_.SetContactFilter(this);
  world_.SetDestructionListener(this);
}

void WorldImpl::Init(event::Queue& queue)
{
  queue.Add(function::Bind(&WorldImpl::Update, shared_from_this()));
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

void WorldImpl::Ambient(float r, float g, float b)
{
  ambient_.Set(float32(r), float32(g), float32(b));
}

float32 WorldImpl::Metres(double pixels) const
{
  return float32(pixels) / scale_;
}

float WorldImpl::Pixels(float32 metres) const
{
  return float(metres * scale_);
}

World::World(json::JSON const& json, collision::Collision& collision, event::Queue& queue)
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

void World::Ambient(float r, float g, float b)
{
  impl_->Ambient(r, g, b);
}

bool World::operator==(World const& other) const
{
  return impl_ == other.impl_;
}
}