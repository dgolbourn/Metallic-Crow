#include "world.h"
#include "world_impl.h"
#include "body_impl.h"
#include "bind.h"
#include "body_impl_iterator.h"
namespace
{
auto ToggleContact(b2Contact* contact, dynamics::BodyImplCount& contacts, bool begin) -> bool
{
  dynamics::BodyImpl* body_a = static_cast<dynamics::BodyImpl*>(contact->GetFixtureA()->GetBody()->GetUserData());
  dynamics::BodyImpl* body_b = static_cast<dynamics::BodyImpl*>(contact->GetFixtureB()->GetBody()->GetUserData());
  dynamics::BodyImplPair pair = dynamics::Make(body_a, body_b);
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
}

namespace dynamics
{
auto WorldImpl::SayGoodbye(b2Joint* joint) -> void
{
  joint->SetUserData(nullptr);
}

auto WorldImpl::SayGoodbye(b2Fixture*) -> void
{
}

auto WorldImpl::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) -> bool
{
  return collision_.Check(BodyImpl::MakeBody(fixtureA->GetBody()), BodyImpl::MakeBody(fixtureB->GetBody()));
}

auto WorldImpl::BeginContact(b2Contact* contact) -> void
{
  if(ToggleContact(contact, contact_, true))
  {
    collision_.Begin(BodyImpl::MakeBody(contact->GetFixtureA()->GetBody()), BodyImpl::MakeBody(contact->GetFixtureB()->GetBody()));
  }
}

auto WorldImpl::EndContact(b2Contact* contact) -> void
{
  if(ToggleContact(contact, contact_, false))
  {
    collision_.End(BodyImpl::MakeBody(contact->GetFixtureA()->GetBody()), BodyImpl::MakeBody(contact->GetFixtureB()->GetBody()));
  }
}

auto WorldImpl::Update() -> void
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

    static const float32 scale = static_cast<float32>(Clock::period::num) / static_cast<float32>(Clock::period::den);
    float32 ds = static_cast<float32>(elapsed.count()) * scale / dt_;

    for(auto& body : world_)
    {
      body.Update(ds);
    }

    end_();
  }
}

auto WorldImpl::Begin(event::Command const& command) -> void
{
  begin_.Add(command);
}

auto WorldImpl::End(event::Command const& command) -> void
{
  end_.Add(command);
}

WorldImpl::WorldImpl(lua::Stack& lua, collision::Group& collision) : world_(b2Vec2(0, 0)), collision_(collision), paused_(true)
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

  f_ = static_cast<float32>(rate);
  double interval = 1. / rate;

  dt_ = static_cast<float32>(interval);
  interval *= static_cast<double>(Clock::period::den) / static_cast<double>(Clock::period::num);
  interval_ = static_cast<Clock::duration>(static_cast<Clock::rep>(interval));
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

auto WorldImpl::Init(event::Queue& queue) -> void
{
  queue.Add(function::Bind(&WorldImpl::Update, shared_from_this()));
}

auto WorldImpl::Pause() -> void
{
  if(!paused_)
  {
    remaining_ = interval_ + tick_ - Clock::now();
    paused_ = true;
  }
}

auto WorldImpl::Resume() -> void
{
  if(paused_)
  {
    tick_ = Clock::now() - interval_ + remaining_;
    paused_ = false;
  }
}

auto WorldImpl::Ambient(float r, float g, float b) -> void
{
  ambient_.Set(static_cast<float32>(r), static_cast<float32>(g), static_cast<float32>(b));
}

auto WorldImpl::Metres(double pixels) const -> float32
{
  return static_cast<float32>(pixels) / scale_;
}

auto WorldImpl::Pixels(float32 metres) const -> float
{
  return static_cast<float>(metres * scale_);
}

World::World(lua::Stack& lua, collision::Group& collision, event::Queue& queue) : impl_(std::make_shared<WorldImpl>(lua, collision))
{
  impl_->Init(queue);
}

auto World::Begin(event::Command const& command) -> void
{
  impl_->Begin(command);
}

auto World::End(event::Command const& command) -> void
{
  impl_->End(command);
}

World::operator bool() const
{
  return bool(impl_);
}

auto World::Pause() -> void
{
  impl_->Pause();
}

auto World::Resume() -> void
{
  impl_->Resume();
}

auto World::Ambient(float r, float g, float b) -> void
{
  impl_->Ambient(r, g, b);
}

auto World::operator==(World const& other) const -> bool
{
  return impl_ == other.impl_;
}
}