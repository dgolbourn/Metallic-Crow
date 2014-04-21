#include "hud.h"
#include "bind.h"
#include <limits>
namespace game
{
class HUDImpl final : public std::enable_shared_from_this<HUDImpl>
{
public:
  HUDImpl(json::JSON const& json, display::Window& window);
  void Score(int score);
  void Life(int life);
  void Render(void) const;
  void Init(Scene& scene);
  display::Window window_;
  sdl::Font font_;
  display::Texture score_;
  display::Texture life_;
  float offset_;
};

HUDImpl::HUDImpl(json::JSON const& json, display::Window& window) : window_(window), offset_(10.f)
{
  json_t* font;
  json.Unpack("{so}",
    "font", &font);
  font_ = sdl::Font(font);
  display::Shape shape = window.Shape();
  Score(0);
  Life(0);
}

void HUDImpl::Score(int score)
{
  score_ = display::Texture(std::to_string(score), font_, window_);
}

void HUDImpl::Life(int life)
{
  life_ = display::Texture(std::to_string(life), font_, window_);
}

void HUDImpl::Render(void) const
{
  score_(display::BoundingBox(), display::BoundingBox(offset_, offset_, 0.f, 0.f), 0.f, false, 0.);
  display::Shape window = window_.Shape();
  display::Shape life = life_.Shape();
  life_(display::BoundingBox(), display::BoundingBox(window.first - life.first - offset_, offset_, 0.f, 0.f), 0.f, false, 0.);
}

void HUDImpl::Init(Scene& scene)
{
  scene.Add(event::Bind(&HUDImpl::Render, shared_from_this()), std::numeric_limits<int>().max());
}

void HUD::Score(int score)
{
  impl_->Score(score);
}

void HUD::Life(int life)
{
  impl_->Life(life);
}

HUD::operator bool(void) const
{
  return bool(impl_);
}

HUD::HUD(json::JSON const& json, display::Window& window, Scene& scene)
{
  impl_ = std::make_shared<HUDImpl>(json, window);
  impl_->Init(scene);
}
}