#include "scenery.h"
#include "bounding_box.h"
#include "bind.h"
#include "json_iterator.h"
#include "exception.h"
#include "animation.h"
#include "timer.h"
namespace game
{
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

class Scenery::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(json::JSON const& json, display::Window& window, int& plane, boost::filesystem::path const& path);
  void Init(Scene& scene, event::Queue&, int plane);
  void Render() const;
  void Next();
  void Modulation(float r, float g, float b);
  event::Timer timer_;
  Animations animations_;
  float parallax_;
  display::Modulation modulation_;
};

void Scenery::Impl::Render(void) const
{
  for(auto& animation : animations_)
  {
    animation.texture_(display::BoundingBox(), animation.render_box_, parallax_, false, 0., modulation_);
  }
}

void Scenery::Impl::Next()
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

void Scenery::Impl::Modulation(float r, float g, float b)
{
  modulation_ = display::Modulation(r, g, b, 1.f);
}

Scenery::Impl::Impl(json::JSON const& json, display::Window& window, int& plane, boost::filesystem::path const& path)
{
  json_t* animations;
  double parallax;
  double r, g, b;
  double interval;
  json.Unpack("{sosfsisfs[fff]}",
    "animations", &animations,
    "interval",  &interval,
    "plane", &plane,
    "parallax", &parallax,
    "modulation" , &r, &g, &b);
 
  modulation_ = display::Modulation(float(r), float(g), float(b), 1.f);
 
  for(json::JSON animation : json::JSON(animations))
  {
    json_t* render_box;
    json_t* animation_ref;
    animation.Unpack("{soso}",
      "animation", &animation_ref,
      "render box", &render_box);
    animations_.emplace_back(display::MakeAnimation(json::JSON(animation_ref), window, path), display::BoundingBox(json::JSON(render_box)));
    if(animations_.back().animation_.empty())
    {
      animations_.pop_back();
    }
  }

  if(animations_.empty())
  {
    interval = -1.;
  }
  timer_ = event::Timer(interval, -1);
}

void Scenery::Impl::Init(Scene& scene, event::Queue& queue, int plane)
{
  queue.Add(function::Bind(&event::Timer::operator(), timer_));
  timer_.Add(function::Bind(&Impl::Next, shared_from_this()));
  scene.Add(function::Bind(&Scenery::Impl::Render, shared_from_this()), plane);
}

Scenery::Scenery(json::JSON const& json, event::Queue& queue, display::Window& window, Scene& scene, boost::filesystem::path const& path)
{
  int plane;
  impl_ = std::make_shared<Impl>(json, window, plane, path);
  impl_->Init(scene, queue, plane);
}

void Scenery::Modulation(float r, float g, float b)
{
  impl_->Modulation(r, g, b);
}
}