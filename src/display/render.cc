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

  ~Modulator()
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
}

namespace sdl
{
auto Render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_FRect const* destination, SDL_FRect const* original, double angle, SDL_Colour const* modulation) -> void
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

auto Render(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source_ptr, SDL_FRect const* destination_ptr, SDL_FPoint view, float zoom, float parallax, bool tile, double angle, SDL_Colour const* modulation, float scale, Angle const* view_angle) -> void
{
  bool render = true;
  SDL_Rect source = {0, 0, texture->w, texture->h};
  SDL_FRect clip = {0.f, 0.f, 1.f, 1.f};
  
  if(source_ptr)
  {
    if(SDL_IntersectRect(source_ptr, &source, &source))
    {
      clip.x = (source.x - source_ptr->x) / static_cast<float>(source_ptr->w);
      clip.y = (source.y - source_ptr->y) / static_cast<float>(source_ptr->h);
      clip.w = source.w / static_cast<float>(source_ptr->w);
      clip.h = source.h / static_cast<float>(source_ptr->h);
    }
    else
    {
      render = false;
    }
  }

  if(render)
  {
    float wf;
    float hf;
    {
      int w, h;
      SDL_GetWindowSize(window, &w, &h);
      wf = static_cast<float>(w);
      hf = static_cast<float>(h);
    }

    SDL_FRect destination;
    if(destination_ptr)
    {
      destination = *destination_ptr;
      
      float w2 = wf * .5f;
      float h2 = hf * .5f;

      if(parallax <= 0.f)
      {
        destination.x *= scale;
        destination.x += w2;
        destination.y *= scale;
        destination.y += h2;
        destination.w *= scale;
        destination.h *= scale;
      }
      else
      {
        scale *= zoom;

        destination.x -= parallax * (view.x - w2);
        destination.x -= w2;
        destination.x *= scale;

        destination.y -= parallax * (view.y - h2);
        destination.y -= h2;
        destination.y *= scale;

        destination.w *= scale;
        destination.h *= scale;    

        float dw = .5f * destination.w;
        float dh = .5f * destination.h;
        double dx = static_cast<double>(destination.x + dw);
        double dy = static_cast<double>(destination.y + dh);
        destination.x = static_cast<float>(dx * view_angle->cos_ - dy * view_angle->sin_) - dw + w2;
        destination.y = static_cast<float>(dx * view_angle->sin_ + dy * view_angle->cos_) - dh + h2;

        angle += view_angle->angle_;  
      }
    }
    else
    {
      destination.x = 0.f;
      destination.y = 0.f;
      destination.w = wf;
      destination.h = hf;
      angle = 0.f;
      tile = false;
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