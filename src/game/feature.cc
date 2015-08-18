#include "feature.h"
#include <unordered_map>
#include "boost/functional/hash.hpp"
namespace
{
typedef std::pair<std::string, int> Key;
typedef std::unordered_map<Key, std::pair<display::Texture, display::Texture>, boost::hash<Key>> TextureMap;
}

namespace game
{
class Feature::Impl
{
public:
  Impl(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path);
  auto Expression(std::string const& expression, int index) -> void;
  auto Expression(std::string const& expression) -> void;
  auto Expression(int index) -> void;
  auto Expression() -> void;
  auto Render(display::BoundingBox const& render_box, display::Modulation const& modulation, float parallax, double angle, bool facing) const -> void;
  TextureMap textures_;
  TextureMap::iterator current_;
  Key state_;
};

Feature::Impl::Impl(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path)
{
  {
    lua::Guard guard = lua.Field("expressions");
    for(int index = 1, end = lua.Size(); index <= end; ++index)
    {
      lua::Guard guard = lua.Field(index);

      std::string expression = lua.Field<std::string>("expression");

      int expession_index = lua.Field<int>("index");
    
      bool facing = lua.Field<bool>("left_facing");
    
      display::BoundingBox clip;
      {
        lua::Guard guard = lua.Field("clip");
        clip = display::BoundingBox(lua);
      }

      display::Texture texture(display::Texture(path / lua.Field<std::string>("page"), window), clip);
      if(facing)
      {
        textures_[Key(expression, expession_index)].first = texture;
      }
      else
      {
        textures_[Key(expression, expession_index)].second = texture;
      }
    }
  }

  state_ = std::make_pair(lua.Field<std::string>("begin_expression"), lua.Field<int>("begin_index"));
  current_ = textures_.find(state_);
}

auto Feature::Impl::Expression(std::string const& expression, int index) -> void
{
  state_.first = expression;
  state_.second = index;
  Expression();
}

auto Feature::Impl::Expression(std::string const& expression) -> void
{
  state_.first = expression;
  Expression();
}

auto Feature::Impl::Expression(int index) -> void
{
  state_.second = index;
  Expression();
}

auto Feature::Impl::Expression() -> void
{
  auto temp = textures_.find(state_);
  if(temp != textures_.end())
  {
    current_ = temp;
  }
  state_.first = current_->first.first;
  state_.second = current_->first.second;
}

auto Feature::Impl::Render(display::BoundingBox const& render_box, display::Modulation const& modulation, float parallax, double angle, bool facing) const -> void
{
  if(current_ != textures_.end())
  {
    if(facing)
    {
      current_->second.first(display::BoundingBox(), render_box, parallax, false, angle, modulation);
    }
    else
    {
      current_->second.second(display::BoundingBox(), render_box, parallax, false, angle, modulation);
    }
  }
}

Feature::Feature(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(lua, window, path))
{
}

auto Feature::Expression(std::string const& expression, int index) -> void
{
  impl_->Expression(expression, index);
}

auto Feature::Expression(std::string const& expression) -> void
{
  impl_->Expression(expression);
}

auto Feature::Expression(int index) -> void
{
  impl_->Expression(index);
}

auto Feature::operator()(display::BoundingBox const& render_box, display::Modulation const& modulation, float parallax, double angle, bool facing) const -> bool
{
  bool valid = bool(impl_);
  if(valid)
  {
    impl_->Render(render_box, modulation, parallax, angle, facing);
  }
  return valid;
}

Feature::operator bool() const
{
  return bool(impl_);
}
}