#include "animation.h"
#include "timer.h"
#include "jansson.h"
#include "json_exception.h"
#include "bounding_box.h"
#include "bind.h"
#include "signal.h"
#include <vector>
namespace game
{
class AnimationImpl : public std::enable_shared_from_this<AnimationImpl>
{
public:
  AnimationImpl(json::JSON const& json, display::Window& window, event::Queue& queue);
  void Next(void);
  void Render(display::BoundingBox const& destination, float parallax, bool tile, double angle) const;
  void Pause(void);
  void Resume(void);
  void Play(int loops, bool end_on_first);
  void End(event::Command const& command);
  void Add(event::Command const& command);
  void Signal(void);
  void EndSignal(void);
  display::Texture texture_;
  std::vector<display::BoundingBox> frames_;
  std::vector<display::BoundingBox>::iterator frame_;
  event::Timer timer_;
  double interval_;
  event::Queue queue_;
  event::Signal signal_;
  event::Signal end_;
};

AnimationImpl::AnimationImpl(json::JSON const& json, display::Window& window, event::Queue& queue) : queue_(queue)
{
  char const* sprite_sheet;
  int interval;
  int width;
  int height;
  json_t* frames;

  json.Unpack("{sssisisiso}", 
    "sprite sheet", &sprite_sheet,
    "interval", &interval,
    "width", &width,
    "height", &height,
    "frames", &frames);

  texture_ = display::Texture(sprite_sheet, window);
  frames_ = std::vector<display::BoundingBox>(json_array_size(frames));
  frame_ = frames_.begin();
  interval_ = double(interval) / 1000.;

  for(json::JSON const& value : json::JSON(frames))
  {
    int x;
    int y;
    json::JSON(value).Unpack("[ii]", &x, &y);
    *frame_++ = display::BoundingBox((float)x, (float)y, (float)width, (float)height);
  }
  frame_ = frames_.begin(); 
}

void AnimationImpl::Next(void)
{
  ++frame_;
  if(frame_ == frames_.end())
  {
    frame_ = frames_.begin();
  }
}

void AnimationImpl::Render(display::BoundingBox const& destination, float parallax, bool tile, double angle) const
{
  texture_(*frame_, destination, parallax, tile, angle);
}

void AnimationImpl::Pause(void)
{
  if(timer_)
  {
    timer_.Pause();
  }
}

void AnimationImpl::Resume(void)
{
  if(timer_)
  {
    timer_.Resume();
  }
}

void AnimationImpl::Play(int loops, bool end_on_first)
{
  int frames = (loops + 1) * frames_.size() - 2 + int(end_on_first);
  timer_ = event::Timer(interval_, frames);
  queue_.Add(event::Bind(&event::Timer::operator(), timer_));
  auto ptr = shared_from_this();
  timer_.Add(event::Bind(&AnimationImpl::Next, ptr));
  timer_.Add(event::Bind(&AnimationImpl::Signal, ptr));
  timer_.End(event::Bind(&AnimationImpl::EndSignal, ptr));
  timer_.Resume();
  frame_ = frames_.begin(); 
}

void AnimationImpl::Signal(void)
{
  signal_();
}

void AnimationImpl::EndSignal(void)
{
  end_();
}

void AnimationImpl::End(event::Command const& command)
{
  end_.Add(command);
}

void AnimationImpl::Add(event::Command const& command)
{
  signal_.Add(command);
}

Animation::Animation(json::JSON const& json, display::Window& window, event::Queue& queue)
{
  impl_ = std::make_shared<AnimationImpl>(json, window, queue);
}

void Animation::Render(display::BoundingBox const& destination, float parallax, bool tile, double angle) const
{
  impl_->Render(destination, parallax, tile, angle);
}

void Animation::Pause(void)
{
  impl_->Pause();
}

void Animation::Resume(void)
{
  impl_->Resume();
}

void Animation::Play(int loops, bool end_on_first)
{
  impl_->Play(loops, end_on_first);
}

void Animation::End(event::Command const& command)
{
  impl_->End(command);
}

void Animation::Add(event::Command const& command)
{
  impl_->Add(command);
}
}