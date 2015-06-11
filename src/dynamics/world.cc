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

void WorldImpl::Update()
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

WorldImpl::WorldImpl(lua::Stack& lua, collision::Collision& collision) : world_(b2Vec2(0, 0)), collision_(collision), paused_(true)
{
  {
    lua::Guard guard = lua.Field("scale");
    lua.Pop(scale_);
  }

  {
    lua::Guard guard = lua.Field("ambient");

    {
      lua::Guard guard = lua.Field(1);
      lua.Pop(ambient_.x);
    }

    {
      lua::Guard guard = lua.Field(2);
      lua.Pop(ambient_.y);
    }

    {
      lua::Guard guard =lua.Field(3);
      lua.Pop(ambient_.z);
    }
  }

  double rate;
  {
    lua::Guard guard = lua.Field("frame_rate");
    lua.Pop(rate);
  }

  f_ = float32(rate);
  double interval = 1. / rate;

  dt_ = float32(interval);
  interval *= double(Clock::period::den) / double(Clock::period::num);
  interval_ = Clock::duration(Clock::rep(interval));
  remaining_ = interval_;

  {
    lua::Guard guard = lua.Field("gravity");

    double f;
    {
      lua::Guard guard = lua.Field(1);
      lua.Pop(f);
    }

    double g;
    {
      lua::Guard guard = lua.Field(2);
      lua.Pop(g);
    }

    world_.SetGravity(b2Vec2(Metres(f), Metres(g)));
  }

  world_.SetAutoClearForces(false);
  world_.SetContactListener(this);
  world_.SetContactFilter(this);
  world_.SetDestructionListener(this);
}

void WorldImpl::Init(event::Queue& queue)
{
  queue.Add(function::Bind(&WorldImpl::Update, shared_from_this()));
}

void WorldImpl::Pause()
{
  if(!paused_)
  {
    remaining_ = interval_ + tick_ - Clock::now();
    paused_ = true;
  }
}

void WorldImpl::Resume()
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

World::World(lua::Stack& lua, collision::Collision& collision, event::Queue& queue) : impl_(std::make_shared<WorldImpl>(lua, collision))
{
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

World::operator bool() const
{
  return bool(impl_);
}

void World::Pause()
{
  impl_->Pause();
}

void World::Resume()
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