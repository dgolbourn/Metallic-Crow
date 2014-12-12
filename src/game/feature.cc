#include "feature.h"
#include <unordered_map>
#include "boost/functional/hash.hpp"
#include "json_iterator.h"
namespace game
{
namespace
{
typedef std::pair<std::string, int> Key;
typedef std::unordered_map<Key, std::pair<display::Texture, display::Texture>, boost::hash<Key>> TextureMap;
}

class Feature::Impl
{
public:
  Impl(json::JSON const& json, display::Window& window, boost::filesystem::path const& path);
  void Expression(std::string const& expression, int index);
  void Expression(std::string const& expression);
  void Expression(int index);
  void Expression();
  void Render(display::BoundingBox const& render_box, display::Modulation const& modulation, float parallax, bool facing) const;
  TextureMap textures_;
  TextureMap::iterator current_;
  Key state_;
};

Feature::Impl::Impl(json::JSON const& json, display::Window& window, boost::filesystem::path const& path)
{
  json_t* expressions;
  char const* begin_expression;
  int begin_index;
  json.Unpack("{sosssi}",
    "expressions", &expressions,
    "begin expression", &begin_expression,
    "begin index", &begin_index);

  for(json::JSON value : json::JSON(expressions))
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

    display::Texture texture(display::Texture(path / page, window), display::BoundingBox(json::JSON(clip)));
    if(facing)
    {
      textures_[Key(expression, index)].first = texture;
    }
    else
    {
      textures_[Key(expression, index)].second = texture;
    }
  }
  state_ = Key(begin_expression, begin_index);
  current_ = textures_.find(state_);
}

void Feature::Impl::Expression(std::string const& expression, int index)
{
  state_.first = expression;
  state_.second = index;
  Expression();
}

void Feature::Impl::Expression(std::string const& expression)
{
  state_.first = expression;
  Expression();
}

void Feature::Impl::Expression(int index)
{
  state_.second = index;
  Expression();
}

void Feature::Impl::Expression()
{
  auto temp = textures_.find(state_);
  if(temp != textures_.end())
  {
    current_ = temp;
  }
  state_.first = current_->first.first;
  state_.second = current_->first.second;
}

void Feature::Impl::Render(display::BoundingBox const& render_box, display::Modulation const& modulation, float parallax, bool facing) const
{
  if(current_ != textures_.end())
  {
    if(facing)
    {
      current_->second.first(display::BoundingBox(), render_box, parallax, false, 0., modulation);
    }
    else
    {
      current_->second.second(display::BoundingBox(), render_box, parallax, false, 0., modulation);
    }
  }
}

Feature::Feature(json::JSON const& json, display::Window& window, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(json, window, path))
{
}

void Feature::Expression(std::string const& expression, int index)
{
  impl_->Expression(expression, index);
}

void Feature::Expression(std::string const& expression)
{
  impl_->Expression(expression);
}

void Feature::Expression(int index)
{
  impl_->Expression(index);
}

bool Feature::operator()(display::BoundingBox const& render_box, display::Modulation const& modulation, float parallax, bool facing) const
{
  bool valid = bool(impl_);
  if(valid)
  {
    impl_->Render(render_box, modulation, parallax, facing);
  }
  return valid;
}

Feature::operator bool() const
{
  return bool(impl_);
}
}