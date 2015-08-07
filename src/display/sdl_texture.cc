#include "sdl_texture.h"
#include "sdl_exception.h"
#include "SDL_image.h"
#include "img_exception.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/global_fun.hpp>
namespace
{
struct TextureRecord
{
  std::weak_ptr<SDL_Texture> texture_;
  boost::filesystem::path path_;
  SDL_Renderer* renderer_;

  TextureRecord(std::shared_ptr<SDL_Texture> const& texture, boost::filesystem::path const& path, SDL_Renderer* renderer) : texture_(texture), path_(path), renderer_(renderer)
  {
  }
};

auto Get(TextureRecord const& record) -> SDL_Texture*
{
  return record.texture_.lock().get();
}

typedef boost::multi_index_container
<
  TextureRecord,
  boost::multi_index::indexed_by
  <
    boost::multi_index::hashed_unique
    <
      boost::multi_index::composite_key
      <
        TextureRecord,
        boost::multi_index::member<TextureRecord, SDL_Renderer*, &TextureRecord::renderer_>,
        boost::multi_index::member<TextureRecord, boost::filesystem::path, &TextureRecord::path_>
      >
    >,
    boost::multi_index::hashed_unique
    <
      boost::multi_index::global_fun
      <
        TextureRecord const&, SDL_Texture*, &Get
      > 
    >
  >
> TextureMap;

TextureMap cache;

auto DestroyTexture(SDL_Texture* texture) noexcept -> void
{
  auto& view = cache.get<1>();
  auto iter = view.find(texture);
  if(iter != view.end())
  {
    view.erase(iter);
  }
  SDL_DestroyTexture(texture);
}
}

namespace sdl
{
Texture::Texture(SDL_Renderer* renderer, SDL_Surface* surface)
{
  SDL_Texture* impl = SDL_CreateTextureFromSurface(renderer, surface);
  if(!impl)
  {
    BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
  }
  impl_ = std::shared_ptr<SDL_Texture>(impl, SDL_DestroyTexture);
}

Texture::Texture(SDL_Renderer* renderer, boost::filesystem::path const& file)
{
  std::shared_ptr<SDL_Texture> impl;
  auto iter = cache.find(boost::make_tuple(renderer, file));
  if(iter != cache.end())
  {
    impl = iter->texture_.lock();
  }

  if(impl)
  {
    impl_ = impl;
  }
  else
  {
    SDL_Texture* impl = IMG_LoadTexture(renderer, file.string().c_str());
    if(!impl)
    {
      BOOST_THROW_EXCEPTION(img::Exception() << img::Exception::What(img::Error()));
    }
    impl_ = std::shared_ptr<SDL_Texture>(impl, DestroyTexture);

    if(iter == cache.end())
    {
      cache.emplace(impl_, file, renderer);
    }
    else
    {
      cache.modify(iter, [&](TextureRecord& record){record.texture_ = impl_;});
    }
  }
}

Texture::operator SDL_Texture*() const
{
  return impl_.get();
}

auto Texture::operator->() const -> SDL_Texture*
{
  return impl_.get();
}

Texture::operator bool() const
{
  return bool(impl_);
}
}