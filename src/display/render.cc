#include "render.h"
#include "sdl_exception.h"
#include "flood_fill.h"
#include "painter.h"
#include "log.h"
namespace
{
class Modulator
{
  SDL_Colour const* modulation_;
  SDL_Colour original_;
  SDL_Texture* texture_;
public:
  Modulator(SDL_Texture* texture, SDL_Colour const* modulation) : modulation_(nullptr)
  {
    if(modulation)
    {
      texture_ = texture; 
      modulation_ = modulation;
      if(SDL_GetTextureColorMod(texture_, &original_.r, &original_.g, &original_.b))
      {
        BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
      }
      if(SDL_GetTextureAlphaMod(texture_, &original_.a))
      {
        BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
      }
      if(SDL_SetTextureColorMod(texture_, modulation->r, modulation->g, modulation->b))
      {
        BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
      }
      if(SDL_SetTextureAlphaMod(texture_, modulation->a))
      {
        BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
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
          BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
        }
        if(SDL_SetTextureAlphaMod(texture_, original_.a))
        {
          BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
        }
      }
    }
    catch(...)
    {
      exception::Log("Swallowed exception");
    }
  }
};

float Transform(float x, float new_origin, float width, float zoom, float parallax)
{
  float w = .5f * width;
  x -= parallax * new_origin;
  x -= w;
  x *= zoom;
  x += w;
  return x;
}
}

namespace sdl
{
void Render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_FRect const* destination, SDL_FRect const* original, double angle, SDL_Colour const* modulation)
{
  SDL_FPoint centre;
  centre.x = .5f * original->w - original->x + destination->x;
  centre.y = .5f * original->h - original->y + destination->y;

  Modulator mod(texture, modulation);
  
  if(texture->native) 
  {
    texture = texture->native;
  }

  if(renderer->RenderCopyEx(renderer, texture, source, destination, angle, &centre, SDL_FLIP_NONE))
  {
    BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
  }
}

void Render(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source_ptr, SDL_FRect const* destination_ptr, SDL_FPoint view, float zoom, float parallax, bool tile, double angle, SDL_Colour const* modulation, float scale)
{
  bool render = true;
  SDL_Rect source = {0, 0, texture->w, texture->h};
  SDL_FRect clip = {0.f, 0.f, 1.f, 1.f};
  
  if(source_ptr)
  {
    if(SDL_IntersectRect(source_ptr, &source, &source))
    {
      clip.x = (source.x - source_ptr->x) / float(source_ptr->w);
      clip.y = (source.y - source_ptr->y) / float(source_ptr->h);
      clip.w = source.w / float(source_ptr->w);
      clip.h = source.h / float(source_ptr->h);
    }
    else
    {
      render = false;
    }
  }

  if(render)
  {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    float wf = float(w);
    float hf = float(h);

    SDL_FRect destination;
    if(destination_ptr)
    {
      if(parallax <= 0.f)
      {
        parallax = 1.f;
        view.x = 0.f;
        view.y = 0.f;
      }
      else
      {
        scale *= zoom;
      }

      destination.x = Transform(destination_ptr->x, view.x - .5f * wf, wf, scale, parallax);
      destination.y = Transform(destination_ptr->y, view.y - .5f * hf, hf, scale, parallax);
      destination.w = scale * destination_ptr->w;
      destination.h = scale * destination_ptr->h;
    }
    else
    {
      destination.x = 0.f;
      destination.y = 0.f;
      destination.w = wf;
      destination.h = hf;
    }

    SDL_FRect original = destination;

    destination.x += clip.x * destination.w;
    destination.y += clip.y * destination.h;
    destination.w *= clip.w;
    destination.h *= clip.h;

    if(tile)
    {
      algorithm::FloodFill<Painter>((Painter(window, renderer, texture, &source, &destination, &original, angle, modulation)));
    }
    else
    {
      Render(renderer, texture, &source, &destination, &original, angle, modulation);
    }
  }
}
}