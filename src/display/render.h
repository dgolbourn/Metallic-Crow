#ifndef RENDER_H_
#define RENDER_H_
#include "SDL_render.h"
#include "SDL_sysrender.h"
namespace sdl
{
auto Render(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_FRect const* destination, SDL_FPoint view, float zoom, float parallax, bool tile, double angle, SDL_Colour const* modulation, float scale) -> void;
auto Render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_FRect const* destination, SDL_FRect const* clip, double angle, SDL_Colour const* modulation) -> void;
}
#endif