#include "screen.h"
#include "timer.h"
#include "animation.h"
#include "bind.h"
namespace game
{
class Screen::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(lua::Stack& lua, display::Window& window, int& plane, boost::filesystem::path const& path);
  void Init(Scene& scene, event::Queue& queue, int plane);
  void Pause();
  void Resume();
  void Next();
  void Render() const;
  void Modulation(float r, float g, float b, float a);
  
  display::Modulation modulation_;
  event::Timer timer_;
  display::Texture texture_;
  display::Animation animation_;
  display::Animation::const_iterator iterator_;
  display::BoundingBox render_box_;
  float parallax_;
  double angle_;
};

Screen::Impl::Impl(lua::Stack& lua, display::Window& window, int& plane, boost::filesystem::path const& path)
{
  {
    lua::Guard guard = lua.Field("angle");
    lua.Pop(angle_);
  }

  {
    lua::Guard guard = lua.Field("modulation");
    modulation_ = display::Modulation(lua);
  }

  {
    lua::Guard guard = lua.Field("parallax");
    lua.Pop(parallax_);
  }

  {
    lua::Guard guard = lua.Field("animation");
    animation_ = display::MakeAnimation(lua, window, path);
  }

  iterator_ = animation_.begin();
  texture_ = *iterator_;

  {
    lua::Guard guard = lua.Field("render_box");
    render_box_ = display::BoundingBox(lua);
  }

  {
    lua::Guard guard = lua.Field("plane");
    lua.Pop(plane);
  }

  double interval;
  if(animation_.empty())
  {
    interval = -1.;
  }
  else
  {
    lua::Guard guard = lua.Field("interval");
    lua.Pop(interval);
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

void Screen::Impl::Modulation(float r, float g, float b, float a)
{
  modulation_.r(r);
  modulation_.g(g);
  modulation_.b(b);
  modulation_.a(a);
}

Screen::Screen(lua::Stack& lua, display::Window& window, Scene& scene, event::Queue& queue, boost::filesystem::path const& path)
{
  int plane;
  impl_ = std::make_shared<Impl>(lua, window, plane, path);
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

void Screen::Modulation(float r, float g, float b, float a)
{
  impl_->Modulation(r, g, b, a);
}
}