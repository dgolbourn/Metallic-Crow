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
  Impl(json::JSON const& json, display::Window& window);
  void Expression(std::string const& expression, int index, bool left_facing);
  void Render(Position const& position, display::Modulation const& modulation) const;
  TextureMap textures_;
  display::Texture texture_;
  display::BoundingBox render_box_;
};

Feature::Impl::Impl(json::JSON const& json, display::Window& window)
{
  json_t* frames;
  json_t* render_box;
  
  json.Unpack("{soso}",
    "render_box", &render_box,
    "frames", &frames);

  render_box_ = display::BoundingBox(json::JSON(render_box));

  for(json::JSON const& value : json::JSON(frames))
  {
    char const* expression;
    int index;
    int facing;
    char const* page;
    json_t* clip;

    json.Unpack("{sssisissso}",
      "expression", &expression,
      "index", &index,
      "left facing", &facing,
      "page", &page,
      "clip", &clip);

    textures_.emplace(Key(expression, index, (facing != 0)), display::Texture(display::Texture(page, window), display::BoundingBox(json::JSON(clip))));
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
  display::BoundingBox box = render_box_.Copy();
  box.x(box.x() + position.first);
  box.y(box.y() + position.second);
  texture_(display::BoundingBox(), box, 1.f, false, 0., modulation);
}

Feature::Feature(json::JSON const& json, display::Window& window) : impl_(std::make_shared<Impl>(json, window))
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