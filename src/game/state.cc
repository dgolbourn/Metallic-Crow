#include "state.h"
#include "animation.h"
#include "bounding_box.h"
#include "sound.h"
namespace game
{
class StateImpl
{
public:
  StateImpl(json::JSON const& json, display::Window& window);
  void Play(void);
  void Pause(void);
  void Resume(void);
  void Stop(void);
  //void Render(Position const& position, bool facing);
  void Render(display::BoundingBox const& bounding_box);
  void End(event::Command const& command);
  Animation animation_;
  display::BoundingBox render_box_;
  audio::Sound sound_;
  display::BoundingBox collision_box_;
  int loops_;
};

StateImpl::StateImpl(json::JSON const& json, display::Window& window)
{
  json_t* animation;
  json_t* render_box;
  json_t* collision_box;
  char const* sound_effect;

  json.Unpack("{sososssosi}",
    "animation", &animation,
    "render box", &render_box,
    "sound effect", &sound_effect,
    "collision box", &collision_box,
    "loops", &loops_);

  animation_ = Animation(animation, window);
  render_box_ = display::BoundingBox(render_box);
  sound_ = audio::Sound(sound_effect);
  collision_box_ = display::BoundingBox(collision_box);
}

void StateImpl::Play(void)
{
  animation_.Play(loops_);
  sound_.Play(loops_);
}

void StateImpl::Pause(void)
{
  animation_.Pause();
  sound_.Pause();
}

void StateImpl::Resume(void)
{
  animation_.Resume();
  sound_.Resume();
}

static const int fade = 1000;

void StateImpl::Stop(void)
{
  animation_.Pause();
  sound_.Fade(fade);
}

void StateImpl::Render(display::BoundingBox const& bounding_box)
{
  animation_.Render(bounding_box, 1.f, false, 0.);
}
//void StateImpl::Render(Position const& position, bool facing)
//{
//  display::BoundingBox render_box = render_box_.Copy();
//  render_box.x(render_box.x() + position.first);
//  render_box.y(render_box.y() + position.second);
//  if(facing)
//  {
//    render_box.x(render_box.x() - 2 * render_box_.x());
//    render_box.w(-render_box.w());
//  }
//  animation_.Render(render_box, 1.f, false, 0.);
//}

void StateImpl::End(event::Command const& command)
{
  animation_.End(command);
}

void State::Play(void)
{
  impl_->Play();
}

void State::Pause(void)
{
  impl_->Pause();
}

void State::Resume(void)
{
  impl_->Resume();
}

void State::Stop(void)
{
  impl_->Stop();
}

void State::End(event::Command const& command)
{
  impl_->End(command);
}

//void State::Render(Position const& position, bool facing)
//{
//  impl_->Render(position, facing);
//}

display::BoundingBox const& State::Collision(void) const
{
  return impl_->collision_box_;
}

display::BoundingBox const& State::Render(void) const
{
  return impl_->render_box_;
}

void State::Render(display::BoundingBox const& bounding_box)
{
  impl_->Render(bounding_box);
}

bool State::operator==(State const& other) const
{
  return impl_ == other.impl_;
}

State::State(void)
{
}

State::State(json::JSON const& json, display::Window& window)
{
  impl_ = std::make_shared<StateImpl>(json, window);
}

State::State(State const& other) : impl_(other.impl_)
{
}

State::State(State&& other) : impl_(std::move(other.impl_))
{
}

State::~State(void)
{
}

State& State::operator=(State other)
{
  std::swap(impl_, other.impl_);
  return *this;
}
}