#include "render.h"
#include "sdl_exception.h"
#include "flood_fill.h"
#include "painter.h"
#include "log.h"
#include "colour.h"
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

class BlendMode
{
  SDL_BlendMode blend_mode_;
  SDL_Texture* texture_;
public:
  BlendMode(SDL_Texture* texture, SDL_BlendMode blend_mode) : texture_(texture)
  {
    if(SDL_GetTextureBlendMode(texture_, &blend_mode_))
    {
      BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
    }
    if(SDL_SetTextureBlendMode(texture_, blend_mode))
    {
      BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
    }
  }

  ~BlendMode()
  {
    try
    {
      if(SDL_SetTextureBlendMode(texture_, blend_mode_))
      {
        BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
      }
    }
    catch(...)
    {
      exception::Log("Swallowed exception");
    }
  }
};
}

namespace display
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
    BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
  }
}

auto Render(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source_ptr, SDL_FRect const* destination_ptr, SDL_FPoint const* view, float zoom, float parallax, bool horizontal, bool vertical, double angle, SDL_Colour const* modulation, float scale, Angle const* view_angle) -> void
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

        destination.x -= parallax * view->x;
        destination.x *= scale;

        destination.y -= parallax * view->y;
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
      horizontal = false;
      vertical = false;
    }

    SDL_FRect original = destination;

    destination.x += clip.x * destination.w;
    destination.y += clip.y * destination.h;
    destination.w *= clip.w;
    destination.h *= clip.h;

    if(horizontal || vertical)
    {
      algorithm::FloodFill<Painter>((Painter(window, renderer, texture, &source, &destination, &original, angle, modulation, horizontal, vertical)), horizontal, vertical);
    }
    else
    {
      Render(renderer, texture, &source, &destination, &original, angle, modulation);
    }
  }
}

auto Render(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, BoundingBox const& source, BoundingBox const& destination, float parallax, bool horizontal, bool vertical, double angle, Modulation const& modulation, float zoom, SDL_FPoint const* view, float scale, Angle const* view_angle) -> void
{
  SDL_Rect const* source_ptr = nullptr;
  SDL_Rect source_copy;
  if(source)
  {
    source_copy.x = static_cast<int>(std::round(source.x()));
    source_copy.y = static_cast<int>(std::round(source.y()));
    source_copy.w = static_cast<int>(std::round(source.w()));
    source_copy.h = static_cast<int>(std::round(source.h()));
    source_ptr = &source_copy;
  }

  SDL_FRect const* destination_ptr = nullptr;
  SDL_FRect destination_copy;
  if(destination)
  {
    destination_copy.x = destination.x();
    destination_copy.y = destination.y();
    destination_copy.w = destination.w();
    destination_copy.h = destination.h();
    destination_ptr = &destination_copy;
  }

  if(modulation)
  {
    SDL_Colour modulation_copy;
    float r = modulation.r();
    float g = modulation.g();
    float b = modulation.b();
    modulation_copy.r = Colour(r--);
    modulation_copy.g = Colour(g--);
    modulation_copy.b = Colour(b--);
    modulation_copy.a = Colour(modulation.a());

    Render(window, renderer, texture, source_ptr, destination_ptr, view, zoom, parallax, horizontal, vertical, angle, &modulation_copy, scale, view_angle);

    if((r > 0.f) || (g > 0.f) || (b > 0.f))
    {      
      BlendMode blend_mode(texture, SDL_BLENDMODE_ADD);
      do
      {
        modulation_copy.r = Colour(r--);
        modulation_copy.g = Colour(g--);
        modulation_copy.b = Colour(b--);
        Render(window, renderer, texture, source_ptr, destination_ptr, view, zoom, parallax, horizontal, vertical, angle, &modulation_copy, scale, view_angle);
      } while((r > 0.f) || (g > 0.f) || (b > 0.f));
    }
  }
  else
  {
    Render(window, renderer, texture, source_ptr, destination_ptr, view, zoom, parallax, horizontal, vertical, angle, nullptr, scale, view_angle);
  }
}
}