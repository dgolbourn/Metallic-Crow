#include "scenery.h"
#include "bounding_box.h"
#include "bind.h"
#include "exception.h"
#include "animation.h"
#include "timer.h"
namespace
{
struct Animation
{
  Animation(display::Animation const& animation, display::BoundingBox const& render_box);
  display::BoundingBox render_box_;
  display::Animation animation_;
  display::Animation::const_iterator iterator_;
  display::Texture texture_;
};

Animation::Animation(display::Animation const& animation, display::BoundingBox const& render_box) : animation_(animation), render_box_(render_box)
{
  iterator_ = animation_.begin();
  if(iterator_ != animation_.end())
  {
    texture_ = *iterator_;
  }
}

typedef std::vector<Animation> Animations;
}

namespace game
{
class Scenery::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(lua::Stack& lua, display::Window& window, int& plane, boost::filesystem::path const& path);
  auto Init(Scene& scene, event::Queue&, int plane) -> void;
  auto Render() const -> void;
  auto Next() -> void;
  auto Modulation(float r, float g, float b, float a) -> void;
  event::Timer timer_;
  Animations animations_;
  float parallax_;
  display::Modulation modulation_;
};

auto Scenery::Impl::Render() const -> void
{
  for(auto& animation : animations_)
  {
    animation.texture_(display::BoundingBox(), animation.render_box_, parallax_, false, 0., modulation_);
  }
}

auto Scenery::Impl::Next() -> void
{
  for(auto& animation : animations_)
  {
    ++animation.iterator_;
    if(animation.iterator_ == animation.animation_.end())
    {
      animation.iterator_ = animation.animation_.begin();
    }
    animation.texture_ = *animation.iterator_;
  }
}

auto Scenery::Impl::Modulation(float r, float g, float b, float a) -> void
{
  modulation_ = display::Modulation(r, g, b, a);
}

Scenery::Impl::Impl(lua::Stack& lua, display::Window& window, int& plane, boost::filesystem::path const& path)
{
  {
    lua::Guard guard = lua.Field("parallax");
    lua.Pop(parallax_);
  }

  {
    lua::Guard guard = lua.Field("plane");
    lua.Pop(plane);
  }

  {
    lua::Guard guard = lua.Field("modulation");
    modulation_ = display::Modulation(lua);
  }
  
  {
    lua::Guard guard = lua.Field("animations");
    for(int index = 1, end = lua.Size(); index <= end; ++index)
    {
      lua::Guard guard = lua.Field(index);

      display::BoundingBox render_box;
      {
        lua::Guard guard = lua.Field("render_box");
        render_box = display::BoundingBox(lua);
      }

      {
        lua::Guard guard = lua.Field("animation");
        animations_.emplace_back(display::MakeAnimation(lua, window, path), render_box);
      }

      if(animations_.back().animation_.empty())
      {
        animations_.pop_back();
      }
    }
  }

  double interval;
  if(animations_.empty())
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

auto Scenery::Impl::Init(Scene& scene, event::Queue& queue, int plane) -> void
{
  queue.Add(function::Bind(&event::Timer::operator(), timer_));
  timer_.Add(function::Bind(&Impl::Next, shared_from_this()));
  scene.Add(function::Bind(&Scenery::Impl::Render, shared_from_this()), plane);
}

Scenery::Scenery(lua::Stack& lua, event::Queue& queue, display::Window& window, Scene& scene, boost::filesystem::path const& path)
{
  int plane;
  impl_ = std::make_shared<Impl>(lua, window, plane, path);
  impl_->Init(scene, queue, plane);
}

auto Scenery::Modulation(float r, float g, float b, float a) -> void
{
  impl_->Modulation(r, g, b, a);
}
}