#include "game_body.h"
#include <unordered_map>
#include <map>
#include <vector>
#include <list>
#include "boost/functional/hash.hpp"
#include "scene.h"
namespace
{
struct Frame
{
  game::Scene scene_;
  std::vector<std::pair<display::BoundingBox, display::BoundingBox>> boxes_;
  double period_;
};

struct Frames
{
  std::vector<Frame> frames_;
  typedef std::pair<std::string, bool> Key;
  typedef std::unordered_map<Key, Frames, boost::hash<Key>> Map;
  Map::iterator next_;
  bool iterruptable_;
};
}

namespace game
{
class Body::Impl
{
public:
  Impl(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path, Feature const& eyes, Feature const& mouth);
  double Expression(std::string const& expression, bool left_facing);
  double Expression(std::string const& expression);
  double Expression(bool left_facing);
  double Expression();
  double Next();
  double Period() const;
  void Position(game::Position const& position);
  game::Position Position() const;
  void Modulation(float r, float g, float b, float a);
  display::Modulation Modulation() const;
  void Render();
 
  Frames::Map expressions_;
  Frames::Map::iterator current_frames_;
  std::vector<Frame>::iterator current_frame_;
  Frames::Map::iterator next_;
  game::Position position_;
  display::Modulation modulation_;
  Frames::Key state_;
};

Body::Impl::Impl(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path, Feature const& eyes_command, Feature const& mouth_command)
{
  {
    lua::Guard guard = lua.Field("modulation");
    modulation_ = display::Modulation(lua);
  }

  std::map<Frames::Key, std::list<Frames::Map::iterator*>> next;
  {
    lua::Guard guard = lua.Field("expressions");
    for(int index = 1, end = lua.Size(); index <= end; ++index)
    {
      lua::Guard guard = lua.Field(index);

      std::string name;
      {
        lua::Guard guard = lua.Field("name");
        lua.Pop(name);
      }

      bool facing;
      {
        lua::Guard guard = lua.Field("left_facing");
        lua.Pop(facing);
      }

      bool interruptable;
      {
        lua::Guard guard = lua.Field("interruptable");
        lua.Pop(interruptable);
      }

      std::string next_expression;
      {
        lua::Guard guard = lua.Field("next_name");
        lua.Pop(next_expression);
      }

      bool next_facing;
      {
        lua::Guard guard = lua.Field("next_left_facing");
        lua.Pop(next_facing);
      }

      Frames& frames = expressions_[Frames::Key(name, facing)];
      frames.iterruptable_ = (interruptable != 0);

      next[Frames::Key(next_expression, next_facing)].push_back(&frames.next_);

      {
        lua::Guard guard = lua.Field("frames");
        frames.frames_.resize(lua.Size());
        for(int index = 1, end = lua.Size(); index <= end; ++index)
        {
          Frame& frame = frames.frames_[index - 1];
          lua::Guard guard = lua.Field(index);

          if(eyes_command)
          {
            bool facing;
            {
              lua::Guard guard = lua.Field("left_facing");
              lua.Pop(facing);
            }

            float eyes_parallax;
            {
              lua::Guard guard = lua.Field("eyes_parallax");
              lua.Pop(eyes_parallax);
            }

            display::BoundingBox eyes;
            {
              lua::Guard guard = lua.Field("eyes_box");
              eyes = display::BoundingBox(lua);
            }

            int plane;
            {
              lua::Guard guard = lua.Field("eyes_plane");
              lua.Pop(plane);
            }

            frame.boxes_.emplace_back(eyes, display::BoundingBox(eyes, display::BoundingBox()));
            frame.scene_.Add([=](){return eyes_command(eyes, modulation_, eyes_parallax, facing); }, plane);
          }

          if(mouth_command)
          {
            bool facing;
            {
              lua::Guard guard = lua.Field("left_facing");
              lua.Pop(facing);
            }

            float mouth_parallax;
            {
              lua::Guard guard = lua.Field("mouth_parallax");
              lua.Pop(mouth_parallax);
            }

            display::BoundingBox mouth;
            {
              lua::Guard guard = lua.Field("mouth_box");
              mouth = display::BoundingBox(lua);
            }

            int plane;
            {
              lua::Guard guard = lua.Field("mouth_plane");
              lua.Pop(plane);
            }

            frame.boxes_.emplace_back(mouth, display::BoundingBox(mouth, display::BoundingBox()));
            frame.scene_.Add([=](){return mouth_command(mouth, modulation_, mouth_parallax, facing); }, plane);
          }

          {
            lua::Guard guard = lua.Field("textures");
            for(int index = 1, end = lua.Size(); index <= end; ++index)
            {
              lua::Guard guard = lua.Field(index);

              display::BoundingBox render;
              {
                lua::Guard guard = lua.Field("render_box");
                render = display::BoundingBox(lua);
              }

              frame.boxes_.emplace_back(render, display::BoundingBox(render, display::BoundingBox()));

              float parallax;
              {
                lua::Guard guard = lua.Field("parallax");
                lua.Pop(parallax);
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

              int plane;
              {
                lua::Guard guard = lua.Field("plane");
                lua.Pop(plane);
              }

              display::Texture texture(display::Texture(path / page, window), clip);
              frame.scene_.Add([=](){return texture(display::BoundingBox(), render, parallax, false, 0., modulation_); }, plane);
            }
          }
        }
      }
    }
  }

  for(auto& list : next)
  {
    auto expression = expressions_.find(list.first);
    for(auto& item : list.second)
    {
      *item = expression;
    }
  }

  bool begin_facing;
  {
    lua::Guard guard = lua.Field("left_facing");
    lua.Pop(begin_facing);
  }

  std::string begin_expression;
  {
    lua::Guard guard = lua.Field("expression");
    lua.Pop(begin_expression);
  }

  {
    lua::Guard guard = lua.Field("position");

    {
      lua::Guard guard = lua.Field(1);
      lua.Pop(position_.first);
    }

    {
      lua::Guard guard = lua.Field(2);
      lua.Pop(position_.second);
    }
  }

  state_ = Frames::Key(begin_expression, begin_facing);
  current_frames_ = expressions_.find(state_);
  current_frame_ = current_frames_->second.frames_.begin();
  next_ = expressions_.end();
  for(auto& box : current_frame_->boxes_)
  {
    box.first.x(box.second.x() + position_.first);
    box.first.y(box.second.y() + position_.second);
  }
}

double Body::Impl::Expression(std::string const& expression, bool left_facing)
{
  state_.first = expression;
  state_.second = left_facing;
  return Expression();
}

double Body::Impl::Expression(std::string const& expression)
{
  state_.first = expression;
  return Expression();
}

double Body::Impl::Expression(bool left_facing)
{
  state_.second = left_facing;
  return Expression();
}

double Body::Impl::Expression()
{
  auto next = expressions_.find(state_);
  if(next != expressions_.end())
  {
    if(next != current_frames_)
    {
      if(current_frames_->second.iterruptable_)
      {
        current_frames_ = next;
        current_frame_ = current_frames_->second.frames_.begin();
        state_.first = current_frames_->first.first;
        state_.second = current_frames_->first.second;
        for(auto& box : current_frame_->boxes_)
        {
          box.first.x(box.second.x() + position_.first);
          box.first.y(box.second.y() + position_.second);
        }
      }
      else
      {
        next_ = next;
      }
    }
  }
  return current_frame_->period_;
}

double Body::Impl::Next()
{
  ++current_frame_;
  if(current_frame_ == current_frames_->second.frames_.end())
  {
    if(!current_frames_->second.iterruptable_ && (next_ != expressions_.end()))
    {
      current_frames_ = next_;
      next_ = expressions_.end();
    }
    else
    {
      current_frames_ = current_frames_->second.next_;
    }
    current_frame_ = current_frames_->second.frames_.begin();
    state_.first = current_frames_->first.first;
    state_.second = current_frames_->first.second;
  }
  for(auto& box : current_frame_->boxes_)
  {
    box.first.x(box.second.x() + position_.first);
    box.first.y(box.second.y() + position_.second);
  }
  return current_frame_->period_;
}

double Body::Impl::Period() const
{
  return current_frame_->period_;
}

void Body::Impl::Render()
{
  current_frame_->scene_.Render();
}

void Body::Impl::Position(game::Position const& position)
{
  position_ = position;
  for(auto& box : current_frame_->boxes_)
  {
    box.first.x(box.second.x() + position_.first);
    box.first.y(box.second.y() + position_.second);
  }
}

game::Position Body::Impl::Position() const
{
  return position_;
}

void Body::Impl::Modulation(float r, float g, float b, float a)
{
  modulation_.r(r);
  modulation_.g(g);
  modulation_.b(b);
  modulation_.a(a);
}

display::Modulation Body::Impl::Modulation() const
{
  return display::Modulation(modulation_.r(), modulation_.g(), modulation_.b(), modulation_.a());
}

Body::Body(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path, Feature const& eyes_command, Feature const& mouth_command) : impl_(std::make_shared<Impl>(lua, window, path, eyes_command, mouth_command))
{
}

double Body::Expression(std::string const& expression, bool left_facing)
{
  return impl_->Expression(expression, left_facing);
}

double Body::Expression(bool left_facing)
{
  return impl_->Expression(left_facing);
}

double Body::Expression(std::string const& expression)
{
  return impl_->Expression(expression);
}

double Body::Next()
{
  return impl_->Next();
}

void Body::Render()
{
  impl_->Render();
}

void Body::Position(game::Position const& position)
{
  impl_->Position(position);
}

game::Position Body::Position() const
{
  return impl_->Position();
}

void Body::Modulation(float r, float g, float b, float a)
{
  impl_->Modulation(r, g, b, a);
}

display::Modulation Body::Modulation() const
{
  return impl_->Modulation();
}

double Body::Period() const
{
  return impl_->Period();
}


Body::operator bool() const
{
  return bool(impl_);
}
}