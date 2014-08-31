#include "state.h"
#include "animation.h"
#include "bounding_box.h"
#include "sound.h"
#include "timer.h"
#include "bind.h"
#include "signal.h"
namespace game
{
class StateImpl : public std::enable_shared_from_this<StateImpl>
{
public:
  StateImpl(json::JSON const& json, display::Window& window);
  void Reset(void);
  void Pause(void);
  void Resume(void);
  void Render(display::BoundingBox const& bounding_box, float parallax, bool tile, double angle, display::Modulation const& modulation) const;
  void End(event::Command const& command);
  void EndSignal();
  void Next();
  void Init(event::Queue& queue);

  display::Texture texture_;
  display::Animation animation_;
  display::Animation::const_iterator iterator_;
  display::BoundingBox render_box_;
  int frames_;
  double interval_;
  event::Timer timer_;
  bool paused_;

  audio::Sound sound_;
  int loops_;
  bool stopped_;

  event::Signal end_;
};

StateImpl::StateImpl(json::JSON const& json, display::Window& window) : paused_(true), stopped_(true)
{
  json_t* animation;
  json_t* render_box;
  json_t* sound_json;
  
  json.Unpack("{sososfsisosi}",
    "animation", &animation,
    "render box", &render_box,
    "interval", &interval_,
    "frames", &frames_,
    "sound effect", &sound_json,
    "sound loops", &loops_);

  animation_ = display::MakeAnimation(json::JSON(animation), window);
  iterator_ = animation_.begin();
  texture_ = *iterator_;
  render_box_ = display::BoundingBox(json::JSON(render_box));
  
  if(auto temp = json::JSON(sound_json))
  {
    char const* sound_file;
    temp.Unpack("s", &sound_file);
    sound_ = audio::Sound(sound_file);
  }
}

void StateImpl::Init(event::Queue& queue)
{
  timer_ = event::Timer(interval_, frames_);
  queue.Add(event::Bind(&event::Timer::operator(), timer_));
  auto ptr = shared_from_this();
  timer_.Add(event::Bind(&StateImpl::Next, ptr));
  timer_.End(event::Bind(&StateImpl::EndSignal, ptr));
}

void StateImpl::EndSignal()
{
  end_();
}

void StateImpl::Next()
{
  ++iterator_;
  if(iterator_ == animation_.end())
  {
    iterator_ = animation_.begin();
  }
  texture_ = *iterator_;
}

void StateImpl::Reset(void)
{
  paused_ = true;
  timer_.Reset(interval_, frames_);
  iterator_ = animation_.begin();
  texture_ = *iterator_;
  if(sound_)
  {
    sound_.Stop();
    stopped_ = true;
  }
}

void StateImpl::Pause(void)
{
  if(!paused_)
  {
    paused_ = true;
    timer_.Pause();
    if(sound_)
    {
      sound_.Pause();
    }
  }
}

void StateImpl::Resume(void)
{
  if(paused_)
  {
    paused_ = false;
    timer_.Resume();

    if(sound_)
    {
      if(stopped_)
      { 
        stopped_ = false;
        sound_.Play(loops_);
      }
      else
      {
        sound_.Resume();
      }
    }
  }
}

void StateImpl::Render(display::BoundingBox const& bounding_box, float parallax, bool tile, double angle, display::Modulation const& modulation) const
{
  (void)texture_(display::BoundingBox(), bounding_box, parallax, tile, angle, modulation);
}

void StateImpl::End(event::Command const& command)
{
  end_.Add(command);
}

void State::Pause(void)
{
  impl_->Pause();
}

void State::Resume(void)
{
  impl_->Resume();
}

void State::Reset(void)
{
  impl_->Reset();
}

void State::End(event::Command const& command)
{
  impl_->End(command);
}

display::BoundingBox const& State::Shape(void) const
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

State::State(json::JSON const& json, display::Window& window, event::Queue& queue) : impl_(std::make_shared<StateImpl>(json, window))
{
  impl_->Init(queue);
}
}