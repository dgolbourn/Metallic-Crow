#include "state.h"
#include "animation.h"
#include "bounding_box.h"
#include "sound.h"
namespace game
{
class StateImpl
{
public:
  StateImpl(json::JSON const& json, display::Window& window, event::Queue& queue);
  void Play(void);
  void Pause(void);
  void Resume(void);
  void Stop(void);
  void Render(display::BoundingBox const& bounding_box, float parallax, bool tile, double angle, display::Modulation const& modulation) const;
  void End(event::Command const& command);
  void Add(event::Command const& command);
  Animation animation_;
  display::BoundingBox render_box_;
  audio::Sound sound_;
  int loops_;
};

StateImpl::StateImpl(json::JSON const& json, display::Window& window, event::Queue& queue)
{
  json_t* animation;
  json_t* render_box;
  char const* sound_effect;

  json.Unpack("{sososssi}",
    "animation", &animation,
    "render box", &render_box,
    "sound effect", &sound_effect,
    "loops", &loops_);

  animation_ = Animation(json::JSON(animation), window, queue);
  render_box_ = display::BoundingBox(json::JSON(render_box));
  if(std::string(sound_effect) != "")
  {
    sound_ = audio::Sound(sound_effect);
  }
}

void StateImpl::Play(void)
{
  animation_.Play(loops_);
  if(sound_)
  {
    sound_.Play(loops_);
  }
}

void StateImpl::Pause(void)
{
  animation_.Pause();
  if(sound_)
  {
    sound_.Pause();
  }
}

void StateImpl::Resume(void)
{
  animation_.Resume();
  if(sound_)
  {
    sound_.Resume();
  }
}

static const int fade = 1000;

void StateImpl::Stop(void)
{
  animation_.Pause();
  if(sound_)
  {
    sound_.Fade(fade);
  }
}

void StateImpl::Render(display::BoundingBox const& bounding_box, float parallax, bool tile, double angle, display::Modulation const& modulation) const
{
  animation_.Render(bounding_box, parallax, tile, angle, modulation);
}

void StateImpl::End(event::Command const& command)
{
  animation_.End(command);
}

void StateImpl::Add(event::Command const& command)
{
  animation_.Add(command);
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

void State::Add(event::Command const& command)
{
  impl_->Add(command);
}

display::BoundingBox const& State::Shape(void)
{
  return impl_->render_box_;
}

void State::Render(display::BoundingBox const& bounding_box, float parallax, bool tile, double angle, display::Modulation const& modulation) const
{
  impl_->Render(bounding_box, parallax, tile, angle, modulation);
}

bool State::operator==(State const& other) const
{
  return impl_ == other.impl_;
}

State::operator bool(void) const
{
  return bool(impl_);
}

State::State(json::JSON const& json, display::Window& window, event::Queue& queue)
{
  impl_ = std::make_shared<StateImpl>(json, window, queue);
}
}