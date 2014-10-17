#include "render.h"
#include "sdl_exception.h"
#include "flood_fill.h"
#include "painter.h"
#include "log.h"
namespace sdl
{
class Modulator
{
  SDL_Color const* modulation_;
  SDL_Color original_;
  SDL_Texture* texture_;
public:
  Modulator(SDL_Texture* texture, SDL_Color const* modulation) : modulation_(nullptr)
  {
    if(modulation)
    {
      texture_ = texture; 
      modulation_ = modulation;
      if(SDL_GetTextureColorMod(texture_, &original_.r, &original_.g, &original_.b))
      {
        BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
      }
      if(SDL_GetTextureAlphaMod(texture_, &original_.a))
      {
        BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
      }
      if(SDL_SetTextureColorMod(texture_, modulation->r, modulation->g, modulation->b))
      {
        BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
      }
      if(SDL_SetTextureAlphaMod(texture_, modulation->a))
      {
        BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
      }
    }
  }

  ~Modulator(void)
  {
    try
    {
      if(modulation_)
      {
        if(SDL_SetTextureColorMod(texture_, original_.r, original_.g, original_.b))
        {
          BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
        }
        if(SDL_SetTextureAlphaMod(texture_, original_.a))
        {
          BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
        }
      }
    }
    catch(...)
    {
      exception::Log("Swallowed exception");
    }
  }
};

void Render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_FRect const* destination, double angle, SDL_Color const* modulation)
{
  SDL_FPoint centre;
  centre.x = .5f * destination->w;
  centre.y = .5f * destination->h;

  Modulator mod(texture, modulation);
  
  if(texture->native) 
  {
    texture = texture->native;
  }

  //if(renderer->hidden == SDL_FALSE)
  {
    if(renderer->RenderCopyEx(renderer, texture, source, destination, angle, &centre, SDL_FLIP_NONE))
    {
      BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
    }
  }
}

static float Transform(float x, float new_origin, float width, float zoom, float parallax)
{
  float w = .5f * width;
  x -= parallax * new_origin;
  x -= w;
  x *= zoom;
  x += w;
  return x;
}

void Render(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source_ptr, SDL_FRect const* destination_ptr, SDL_FPoint const* view, float zoom, float parallax, bool tile, double angle, SDL_Color const* modulation)
{
  bool render = true;
  SDL_Rect source = {0, 0, texture->w, texture->h};
  if(source_ptr)
  {
    if(SDL_IntersectRect(source_ptr, &source, &source) == SDL_FALSE)
    {
      render = false;
    }
  }

  if(render)
  {
    SDL_FRect destination;
    if(destination_ptr)
    {
      destination.x = destination_ptr->x;
      destination.y = destination_ptr->y;
      destination.w = destination_ptr->w;
      destination.h = destination_ptr->h;
      if(destination.w == 0.f)
      {
        destination.w = float(texture->w);
      }
      if(destination.h == 0.f)
      {
        destination.h = float(texture->h);
      }

      if(parallax > 0.f)
      {
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        destination.x = Transform(destination.x, view->x, float(w), zoom, parallax);
        destination.y = Transform(destination.y, view->y, float(h), zoom, parallax);
        destination.w = zoom * destination.w;
        destination.h = zoom * destination.h;
      }
    }
    else
    {
      int w, h;
      SDL_GetWindowSize(window, &w, &h);
      destination.x = 0.f;
      destination.y = 0.f;
      destination.w = float(w);
      destination.h = float(h);
    }

    if(tile)
    {
      algorithm::FloodFill<Painter>()(Painter(window, renderer, texture, &source, &destination, angle, modulation));
    }
    else
    {
      Render(renderer, texture, &source, &destination, angle, modulation);
    }
  }
}
}