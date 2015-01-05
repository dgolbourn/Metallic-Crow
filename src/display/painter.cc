#include "painter.h"
#include "render.h"
#include "rect.h"
#include <cmath>
namespace sdl
{
class PainterImpl
{
public:
  PainterImpl(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_FRect const* destination, SDL_FRect const* original, double angle, SDL_Colour const* modulation);
  bool operator()(algorithm::NodeCoordinates const& coords);
  void MovementVectors();
  void CollisionBox();
  void StartPosition();
  SDL_Renderer* renderer_;
  SDL_Texture* texture_;
  SDL_Rect const* source_;
  SDL_FPoint north_;
  SDL_FPoint east_;
  SDL_FRect view_;
  SDL_FRect collision_;
  SDL_FRect destination_;
  SDL_FRect original_;
  SDL_Colour const* modulation_;
  double angle_;
  float c_;
  float s_;
};

void PainterImpl::CollisionBox()
{
  collision_.w = std::max(std::abs(east_.x - north_.x), std::abs(east_.x + north_.x));
  collision_.h = std::max(std::abs(east_.y - north_.y), std::abs(east_.y + north_.y));
  collision_.x = original_.x + .5f * (original_.w - collision_.w);
  collision_.y = original_.y + .5f * (original_.h - collision_.h);
}

void PainterImpl::StartPosition()
{
  float a = std::round((c_ * original_.x + s_ * original_.y) / original_.w);
  float b = std::round((-s_ * original_.x + c_ * original_.y) / original_.h);
  SDL_FRect collision;
  collision.w = collision_.w;
  collision.h = collision_.h;
  static float const sign[] = {0.f, 1.f, -1.f};
  for(float i : sign)
  {
    bool break_flag = false;
    for(float j : sign)
    {
      SDL_FPoint adjust;
      adjust.x = (a + i) * east_.x + (b + j) * north_.x;
      adjust.y = (a + i) * east_.y + (b + j) * north_.y;
      collision.x = collision_.x - adjust.x;
      collision.y = collision_.y - adjust.y;
      if(Intersection(&collision, &view_))
      {
        collision_.x = collision.x;
        collision_.y = collision.y;
        destination_.x -= adjust.x;
        destination_.y -= adjust.y;
        original_.x -= adjust.x;
        original_.y -= adjust.y;
        break_flag = true;
        break;
      }
    }
    if(break_flag)
    {
      break;
    }
  }
}

void PainterImpl::MovementVectors()
{
  double angle = angle_ * M_PI / 180.;
  c_ = (float)std::cos(angle);
  s_ = (float)std::sin(angle);
  east_.x = c_ * original_.w;
  east_.y = s_ * original_.w;
  north_.x = -s_ * original_.h;
  north_.y = c_ * original_.h;
}

PainterImpl::PainterImpl(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_FRect const* destination, SDL_FRect const* original, double angle, SDL_Colour const* modulation)
{
  renderer_ = renderer;
  texture_ = texture;
  source_ = source;
  destination_ = *destination;
  angle_ = angle;
  modulation_ = modulation;
  original_ = *original;
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  view_.w = (float)w;
  view_.h = (float)h;
  view_.x = 0.f;
  view_.y = 0.f;
  MovementVectors();
  CollisionBox();
  StartPosition();
}

bool PainterImpl::operator()(algorithm::NodeCoordinates const& coords)
{
  bool filled = false;
  float east = float(coords.first);
  float north = float(coords.second);
  SDL_FPoint move;
  move.x = east_.x * east + north_.x * north;
  move.y = east_.y * east + north_.y * north;
  SDL_FRect collision = collision_;
  collision.x += move.x;
  collision.y += move.y;
  if(Intersection(&collision, &view_))
  {
    SDL_FRect destination = destination_;
    destination.x += move.x;
    destination.y += move.y;
    SDL_FRect original = original_;
    original.x += move.x;
    original.y += move.y;
    Render(renderer_, texture_, source_, &destination, &original, angle_, modulation_);
    filled = true;
  }
  return filled;
}

bool Painter::operator()(algorithm::NodeCoordinates const& coords)
{
  return (*impl_)(coords);
}

Painter::Painter(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_FRect const* destination, SDL_FRect const* original, double angle, SDL_Colour const* modulation)
{
  impl_ = std::make_shared<PainterImpl>(window, renderer, texture, source, destination, original, angle, modulation);
}
}