#include "game_animation.h"
#include "animation.h"
#include "bounding_box.h"
#include "timer.h"
#include "bind.h"
#include "signal.h"
namespace game
{
class Animation::Impl : public std::enable_shared_from_this<Impl>
{
public:
  Impl(json::JSON const& json, display::Window& window);
  void Reset(void);
  void Pause(void);
  void Resume(void);
  void Render(display::BoundingBox const& source, display::BoundingBox const& destination, float parallax, bool tile, double angle, display::Modulation const& modulation) const;
  void Add(event::Command const& command);
  void Next();
  void Init(event::Queue& queue);

  display::Texture texture_;
  display::Animation animation_;
  display::Animation::const_iterator iterator_;
  int frames_;
  double interval_;
  event::Timer timer_;
  event::Signal signal_;
};

Animation::Impl::Impl(json::JSON const& json, display::Window& window)
{
  json_t* animation;

  json.Unpack("{sososfsisosi}",
    "animation", &animation,
    "interval", &interval_,
    "frames", &frames_);

  animation_ = display::MakeAnimation(json::JSON(animation), window);
  iterator_ = animation_.begin();
  texture_ = *iterator_;
}

void Animation::Impl::Init(event::Queue& queue)
{
  timer_ = event::Timer(interval_, frames_);
  queue.Add(function::Bind(&event::Timer::operator(), timer_));
  timer_.Add(function::Bind(&Animation::Impl::Next, shared_from_this()));
  typedef void (event::Signal::*Notify)();
  timer_.End(function::Bind((Notify)&event::Signal::operator(), signal_));
}

void Animation::Impl::Next()
{
  ++iterator_;
  if(iterator_ == animation_.end())
  {
    iterator_ = animation_.begin();
  }
  texture_ = *iterator_;
}

void Animation::Impl::Reset(void)
{
  timer_.Reset(interval_, frames_);
  iterator_ = animation_.begin();
  texture_ = *iterator_;
}

void Animation::Impl::Pause(void)
{
  timer_.Pause();
}

void Animation::Impl::Resume(void)
{
  timer_.Resume();
}

void Animation::Impl::Render(display::BoundingBox const& source, display::BoundingBox const& destination, float parallax, bool tile, double angle, display::Modulation const& modulation) const
{
  texture_(source, destination, parallax, tile, angle, modulation);
}

void Animation::Impl::Add(event::Command const& command)
{
  signal_.Add(command);
}

void Animation::Pause(void)
{
  impl_->Pause();
}

void Animation::Resume(void)
{
  impl_->Resume();
}

void Animation::Reset(void)
{
  impl_->Reset();
}

void Animation::Add(event::Command const& command)
{
  impl_->Add(command);
}

void Animation::operator()(display::BoundingBox const& source, display::BoundingBox const& destination, float parallax, bool tile, double angle, display::Modulation const& modulation) const
{
  impl_->Render(source, destination, parallax, tile, angle, modulation);
}

Animation::operator bool(void) const
{
  return bool(impl_);
}

Animation::Animation(json::JSON const& json, display::Window& window, event::Queue& queue) : impl_(std::make_shared<Impl>(json, window))
{
  impl_->Init(queue);
}
}