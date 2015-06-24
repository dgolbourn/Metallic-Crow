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
  auto Init(Scene& scene, event::Queue& queue, int plane) -> void;
  auto Pause() -> void;
  auto Resume() -> void;
  auto Next() -> void;
  auto Render() const -> void;
  auto Modulation(float r, float g, float b, float a) -> void;
  
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

auto Screen::Impl::Init(Scene& scene, event::Queue& queue, int plane) -> void
{
  queue.Add(function::Bind(&event::Timer::operator(), timer_));
  timer_.Add(function::Bind(&Impl::Next, shared_from_this()));
  scene.Add(function::Bind(&Impl::Render, shared_from_this()), plane);
}

auto Screen::Impl::Render() const -> void
{
  texture_(display::BoundingBox(), render_box_, parallax_, true, angle_, modulation_);
}

auto Screen::Impl::Next() -> void
{
  ++iterator_;
  if(iterator_ == animation_.end())
  {
    iterator_ = animation_.begin();
  }
  texture_ = *iterator_;
}

auto Screen::Impl::Pause() -> void
{
  timer_.Pause();
}

auto Screen::Impl::Resume() -> void
{
  timer_.Resume();
}

auto Screen::Impl::Modulation(float r, float g, float b, float a) -> void
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

auto Screen::Pause() -> void
{
  impl_->Pause();
}

auto Screen::Resume() -> void
{
  impl_->Resume();
}

auto Screen::Modulation(float r, float g, float b, float a) -> void
{
  impl_->Modulation(r, g, b, a);
}
}