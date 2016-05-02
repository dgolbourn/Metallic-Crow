#ifndef RENDER_H_
#define RENDER_H_
#include "SDL_render.h"
#include "SDL_sysrender.h"
#include "bounding_box.h"
#include "modulation.h"
namespace display
{
struct Angle
{
  double angle_;
  double sin_;
  double cos_;
};

auto Render(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_FRect const* destination, SDL_FPoint view, float zoom, float parallax, bool horizontal, bool vertical, double angle, SDL_Colour const* modulation, float scale, Angle const* view_angle) -> void;
auto Render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_FRect const* destination, SDL_FRect const* clip, double angle, SDL_Colour const* modulation) -> void;
auto Render(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, BoundingBox const& source, BoundingBox const& destination, float parallax, bool horizontal, bool vertical, double angle, Modulation const& modulation, float zoom, SDL_FPoint const* view, float scale, Angle const* view_angle) -> void;
}
#endif