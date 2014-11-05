#include "feature.h"
#include <unordered_map>
#include "boost/functional/hash.hpp"
#include "json_iterator.h"
namespace game
{
namespace
{
typedef std::tuple<std::string, int, bool> Key;
typedef std::unordered_map<Key, display::Texture, boost::hash<Key>> TextureMap;
}

class Feature::Impl
{
public:
  Impl(json::JSON const& json, display::Window& window, boost::filesystem::path const& path);
  void Expression(std::string const& expression, int index, bool left_facing);
  void Render(Position const& position, display::Modulation const& modulation) const;
  TextureMap textures_;
  display::Texture texture_;
  display::BoundingBox render_box_;
};

Feature::Impl::Impl(json::JSON const& json, display::Window& window, boost::filesystem::path const& path)
{
  json_t* expressions;
  json_t* render_box;
  
  json.Unpack("{soso}",
    "render box", &render_box,
    "expressions", &expressions);

  render_box_ = display::BoundingBox(json::JSON(render_box));

  for(json::JSON const& value : json::JSON(expressions))
  {
    char const* expression;
    int index;
    int facing;
    char const* page;
    json_t* clip;

    json.Unpack("{sssisbssso}",
      "expression", &expression,
      "index", &index,
      "left facing", &facing,
      "page", &page,
      "clip", &clip);

    textures_.emplace(Key(expression, index, (facing != 0)), display::Texture(display::Texture(path / page, window), display::BoundingBox(json::JSON(clip))));
  }
}

void Feature::Impl::Expression(std::string const& expression, int index, bool left_facing)
{
  auto temp = textures_.find(Key(expression, index, left_facing));
  if(temp != textures_.end())
  {
    texture_ = temp->second;
  }
}

void Feature::Impl::Render(Position const& position, display::Modulation const& modulation) const
{
  display::BoundingBox box = display::BoundingBox(render_box_, display::BoundingBox());
  box.x(box.x() + position.first);
  box.y(box.y() + position.second);
  texture_(display::BoundingBox(), box, 1.f, false, 0., modulation);
}

Feature::Feature(json::JSON const& json, display::Window& window, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(json, window, path))
{
}

void Feature::Expression(std::string const& expression, int index, bool left_facing)
{
  impl_->Expression(expression, index, left_facing);
}

void Feature::Render(Position const& position, display::Modulation const& modulation) const
{
  impl_->Render(position, modulation);
}
}