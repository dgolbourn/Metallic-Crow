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
  void Render(void);
  void Init(Scene& scene);
  display::Window window_;
  sdl::Font font_;
  display::Texture score_;
  display::Texture life_;
  display::BoundingBox score_position_;
  display::BoundingBox life_position_;
};

HUDImpl::HUDImpl(json::JSON const& json, display::Window& window) : window_(window)
{
  double score_x;
  double score_y;
  double life_x;
  double life_y;
  json_t* font;

  json.Unpack("{sos[ff]s[ff]}",
    "font", &font,
    "score position", &score_x, &score_y,
    "life position", &life_x, &life_y);

  font_ = sdl::Font(font);
  score_position_ = display::BoundingBox((float)score_x, (float)score_y, 0.f, 0.f);
  life_position_ = display::BoundingBox((float)life_x, (float)life_y, 0.f, 0.f);
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

void HUDImpl::Render(void) 
{
  score_(display::BoundingBox(), score_position_, 0.f, false, 0.);
  life_(display::BoundingBox(), life_position_, 0.f, false, 0.);
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