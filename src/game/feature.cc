#include "feature.h"
#include <unordered_map>
#include <map>
#include "boost/functional/hash.hpp"
namespace
{
typedef std::pair<std::string, int> Key;
typedef std::unordered_map<Key, std::map<int, display::Texture>, boost::hash<Key>> TextureMap;
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
  auto Render(display::BoundingBox const& render_box, display::Modulation const& modulation, float parallax, double angle, int direction) const -> void;
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
    
      int direction = lua.Field<int>("direction");
    
      display::BoundingBox clip;
      {
        lua::Guard guard = lua.Field("clip");
        clip = display::BoundingBox(lua);
      }

      display::Texture texture(path / lua.Field<std::string>("page"), window, clip);
      
      textures_[Key(expression, expession_index)][direction] = texture; 
    }
  }

  state_ = Key(lua.Field<std::string>("begin_expression"), lua.Field<int>("begin_index"));
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

auto Feature::Impl::Render(display::BoundingBox const& render_box, display::Modulation const& modulation, float parallax, double angle, int direction) const -> void
{
  if(current_ != textures_.end())
  {
    current_->second[direction](display::BoundingBox(), render_box, parallax, false, false, angle, modulation);
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

auto Feature::operator()(display::BoundingBox const& render_box, display::Modulation const& modulation, float parallax, double angle, int direction) const -> bool
{
  bool valid = static_cast<bool>(impl_);
  if(valid)
  {
    impl_->Render(render_box, modulation, parallax, angle, direction);
  }
  return valid;
}

Feature::operator bool() const
{
  return static_cast<bool>(impl_);
}
}