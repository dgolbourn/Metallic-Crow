#include "screen.h"
#include "timer.h"
#include "animation.h"
#include "bind.h"
namespace game
{
class Screen::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(json::JSON const& json, display::Window& window, int& plane, boost::filesystem::path const& path);
  void Init(Scene& scene, event::Queue& queue, int plane);
  void Pause();
  void Resume();
  void Next();
  void Render() const;
  void Modulation(float r, float g, float b);
  
  display::Modulation modulation_;
  event::Timer timer_;
  display::Texture texture_;
  display::Animation animation_;
  display::Animation::const_iterator iterator_;
  display::BoundingBox render_box_;
  float parallax_;
  double angle_;
};

Screen::Impl::Impl(json::JSON const& json, display::Window& window, int& plane, boost::filesystem::path const& path) : modulation_(1.f, 1.f, 1.f, 1.f)
{
  json_t* animation;
  json_t* render_box;
  double parallax;
  double interval;

  json.Unpack("{sososfsisfsf}",
    "animation", &animation,
    "render box", &render_box,
    "interval", &interval,
    "plane", &plane,
    "parallax", &parallax,
    "angle", &angle_);

  parallax_ = float(parallax);
  animation_ = display::MakeAnimation(json::JSON(animation), window, path);
  iterator_ = animation_.begin();
  texture_ = *iterator_;
  render_box_ = display::BoundingBox(json::JSON(render_box));
  if(animation_.empty())
  {
    interval = -1.;
  }
  timer_ = event::Timer(interval, -1);
}

void Screen::Impl::Init(Scene& scene, event::Queue& queue, int plane)
{
  queue.Add(function::Bind(&event::Timer::operator(), timer_));
  timer_.Add(function::Bind(&Impl::Next, shared_from_this()));
  scene.Add(function::Bind(&Impl::Render, shared_from_this()), plane);
}

void Screen::Impl::Render() const
{
  texture_(display::BoundingBox(), render_box_, parallax_, true, angle_, modulation_);
}

void Screen::Impl::Next()
{
  ++iterator_;
  if(iterator_ == animation_.end())
  {
    iterator_ = animation_.begin();
  }
  texture_ = *iterator_;
}

void Screen::Impl::Pause()
{
  timer_.Pause();
}

void Screen::Impl::Resume()
{
  timer_.Resume();
}

void Screen::Impl::Modulation(float r, float g, float b)
{
  modulation_.r(r);
  modulation_.g(g);
  modulation_.b(b);
}

Screen::Screen(json::JSON const& json, display::Window& window, Scene& scene, event::Queue& queue, boost::filesystem::path const& path)
{
  int plane;
  impl_ = std::make_shared<Impl>(json, window, plane, path);
  impl_->Init(scene, queue, plane);
}

void Screen::Pause()
{
  impl_->Pause();
}

void Screen::Resume()
{
  impl_->Resume();
}

void Screen::Modulation(float r, float g, float b)
{
  impl_->Modulation(r, g, b);
}
}