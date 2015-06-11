#include "feature.h"
#include <unordered_map>
#include "boost/functional/hash.hpp"

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
  Impl(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path);
  void Expression(std::string const& expression, int index);
  void Expression(std::string const& expression);
  void Expression(int index);
  void Expression();
  void Render(display::BoundingBox const& render_box, display::Modulation const& modulation, float parallax, bool facing) const;
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

      std::string expression;
      {
        lua::Guard guard = lua.Field("expression");
        lua.Pop(expression);
      }

      int expession_index;
      {
        lua::Guard guard = lua.Field("index");
        lua.Pop(expession_index);
      }

      bool facing;
      {
        lua::Guard guard = lua.Field("left_facing");
        lua.Pop(facing);
      }

      std::string page;
      {
        lua::Guard guard = lua.Field("page");
        lua.Pop(page);
      }

      display::BoundingBox clip;
      {
        lua::Guard guard = lua.Field("clip");
        clip = display::BoundingBox(lua);
      }

      display::Texture texture(display::Texture(path / page, window), clip);
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

  std::string begin_expression;
  {
    lua::Guard guard = lua.Field("begin_expression");
    lua.Pop(begin_expression);
  }

  int begin_index;
  {
    lua::Guard guard = lua.Field("begin_index");
    lua.Pop(begin_index);
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

Feature::Feature(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(lua, window, path))
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