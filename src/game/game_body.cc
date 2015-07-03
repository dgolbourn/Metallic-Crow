#include "game_body.h"
#include <unordered_map>
#include <map>
#include <vector>
#include <list>
#include "boost/functional/hash.hpp"
#include "scene.h"
#include "SDL_stdinc.h"
namespace
{
struct Renderable
{
  display::BoundingBox current_box_;
  double current_angle_;
  display::BoundingBox box_;
  double angle_;
  display::Modulation modulation_;
  float parallax_;
  bool facing_;
  virtual auto operator()() const -> void = 0;
};

struct Feature final : public Renderable
{
  game::Feature feature_;
  auto operator()() const -> void override
  {
    feature_(current_box_, modulation_, parallax_, current_angle_, facing_);
  }
};

struct Texture final : public Renderable
{
  display::Texture texture_;
  auto operator()() const -> void override
  {
    texture_(display::BoundingBox(), current_box_, parallax_, false, current_angle_, modulation_);
  }
};

typedef std::shared_ptr<Renderable> RenderPtr;

struct Frame
{
  std::multimap<int, RenderPtr> scene_; 
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

auto Rotate(double cos, double sin, display::BoundingBox const& box) -> game::Position
{
  float w = box.w() * .5f;
  float h = box.h() * .5f;
  double dx = static_cast<double>(box.x() + w);
  double dy = static_cast<double>(box.y() + h);
  game::Position position;
  position.first = static_cast<float>(dx * cos - dy * sin) - w;
  position.second = static_cast<float>(dx * sin + dy * cos) - h;
  return position;
}
}

namespace game
{
class Body::Impl
{
public:
  Impl(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path, Feature const& eyes, Feature const& mouth);
  auto Expression(std::string const& expression, bool left_facing) -> double;
  auto Expression(std::string const& expression) -> double;
  auto Expression(bool left_facing) -> double;
  auto Expression() -> double;
  auto Next() -> double;
  auto Period() const -> double;
  auto Position(game::Position const& position) -> void;
  auto Position() const -> game::Position;
  auto Modulation(float r, float g, float b, float a) -> void;
  auto Modulation() const -> display::Modulation;
  auto Render() const -> void;
  auto Rotation(double angle) -> void;
  auto Rotation() const -> double;
  auto UpdateFrame() -> void;
 
  Frames::Map expressions_;
  Frames::Map::iterator current_frames_;
  std::vector<Frame>::iterator current_frame_;
  Frames::Map::iterator next_;
  game::Position position_;
  display::Modulation modulation_;
  Frames::Key state_;
  double angle_;
  double sin_;
  double cos_;
};

Body::Impl::Impl(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path, Feature const& eyes_command, Feature const& mouth_command) : angle_(0.), sin_(0.), cos_(1.)
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
            RenderPtr render = std::make_unique<::Feature>();

            {
              lua::Guard guard = lua.Field("left_facing");
              lua.Pop(render->facing_);
            }

            {
              lua::Guard guard = lua.Field("eyes_parallax");
              lua.Pop(render->parallax_);
            }

            {
              lua::Guard guard = lua.Field("eyes_box");
              render->box_ = display::BoundingBox(lua);
              render->current_box_ = display::BoundingBox(lua);
            }

            int plane;
            {
              lua::Guard guard = lua.Field("eyes_plane");
              lua.Pop(plane);
            }

            {
              double angle = 0.;
              lua::Guard guard = lua.Field("eyes_angle");
              if(lua.Check())
              {
                lua.Pop(angle);
              }
              render->angle_ = angle;
              render->current_angle_ = angle;
            }

            render->modulation_ = modulation_;

            static_cast<::Feature*>(render.get())->feature_ = eyes_command;

            frame.scene_.emplace(plane, std::move(render));
          }

          if(mouth_command)
          {
            RenderPtr render = std::make_unique<::Feature>();

            {
              lua::Guard guard = lua.Field("left_facing");
              lua.Pop(render->facing_);
            }

            {
              lua::Guard guard = lua.Field("mouth_parallax");
              lua.Pop(render->parallax_);
            }

            {
              lua::Guard guard = lua.Field("mouth_box");
              render->current_box_ = display::BoundingBox(lua);
              render->box_ = display::BoundingBox(lua);
            }

            int plane;
            {
              lua::Guard guard = lua.Field("mouth_plane");
              lua.Pop(plane);
            }

            {
              double angle = 0.;
              lua::Guard guard = lua.Field("mouth_angle");
              if(lua.Check())
              {
                lua.Pop(angle);
              }
              render->angle_ = angle;
              render->current_angle_ = angle;
            }

            render->modulation_ = modulation_;

            static_cast<::Feature*>(render.get())->feature_ = mouth_command;

            frame.scene_.emplace(plane, std::move(render));
          }

          {
            lua::Guard guard = lua.Field("period");
            lua.Pop(frame.period_);
          }

          {
            lua::Guard guard = lua.Field("textures");
            for(int index = 1, end = lua.Size(); index <= end; ++index)
            {
              lua::Guard guard = lua.Field(index);

              RenderPtr render = std::make_unique<::Texture>();

              {
                lua::Guard guard = lua.Field("render_box");
                render->current_box_ = display::BoundingBox(lua);
                render->box_ = display::BoundingBox(lua);
              }

              {
                lua::Guard guard = lua.Field("parallax");
                lua.Pop(render->parallax_);
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

              {
                double angle = 0.;
                lua::Guard guard = lua.Field("angle");
                if(lua.Check())
                {
                  lua.Pop(angle);
                }
                render->angle_ = angle;
                render->current_angle_ = angle;
              }
  
              render->modulation_ = modulation_;

              static_cast<::Texture*>(render.get())->texture_ = display::Texture(display::Texture(path / page, window), clip);
              frame.scene_.emplace(plane, std::move(render));
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

  {
    lua::Guard guard = lua.Field("angle");
    if(lua.Check()) 
    {
      lua.Pop(angle_);
      double angle = angle_ * M_PI / 180.;
      cos_ = std::cos(angle);
      sin_ = std::sin(angle);
    }
  }

  state_ = Frames::Key(begin_expression, begin_facing);
  current_frames_ = expressions_.find(state_);
  current_frame_ = current_frames_->second.frames_.begin();
  next_ = expressions_.end();

  UpdateFrame();
}

auto Body::Impl::UpdateFrame() -> void
{
  for(auto& render : current_frame_->scene_)
  {
    ::Renderable& texture = *render.second;
    game::Position rotated = Rotate(cos_, sin_, texture.box_);
    texture.current_box_.x(rotated.first + position_.first);
    texture.current_box_.y(rotated.second + position_.second);
    texture.current_angle_ = texture.angle_ + angle_;
  }
}

auto Body::Impl::Expression(std::string const& expression, bool left_facing) -> double
{
  state_.first = expression;
  state_.second = left_facing;
  return Expression();
}

auto Body::Impl::Expression(std::string const& expression) -> double
{
  state_.first = expression;
  return Expression();
}

auto Body::Impl::Expression(bool left_facing) -> double
{
  state_.second = left_facing;
  return Expression();
}

auto Body::Impl::Expression() -> double
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
        UpdateFrame();
      }
      else
      {
        next_ = next;
      }
    }
  }
  return current_frame_->period_;
}

auto Body::Impl::Next() -> double
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
  
  UpdateFrame();

  return current_frame_->period_;
}

auto Body::Impl::Period() const -> double
{
  return current_frame_->period_;
}

auto Body::Impl::Render() const -> void
{
  for(auto& render : current_frame_->scene_)
  {
    (*render.second)();
  }
}

auto Body::Impl::Position(game::Position const& position) -> void
{
  position_ = position;
  for(auto& render : current_frame_->scene_)
  {
    ::Renderable& texture = *render.second;
    game::Position rotated = Rotate(cos_, sin_, texture.box_);
    texture.current_box_.x(rotated.first + position_.first);
    texture.current_box_.y(rotated.second + position_.second);
  }
}

auto Body::Impl::Position() const -> game::Position
{
  return position_;
}

auto Body::Impl::Modulation(float r, float g, float b, float a) -> void
{
  modulation_.r(r);
  modulation_.g(g);
  modulation_.b(b);
  modulation_.a(a);
}

auto Body::Impl::Modulation() const -> display::Modulation
{
  return display::Modulation(modulation_.r(), modulation_.g(), modulation_.b(), modulation_.a());
}

auto Body::Impl::Rotation(double angle) -> void
{
  angle_ = angle;
  angle = angle_ * M_PI / 180.;
  cos_ = std::cos(angle);
  sin_ = std::sin(angle);
  UpdateFrame();
}

auto Body::Impl::Rotation() const -> double
{
  return angle_;
}

Body::Body(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path, Feature const& eyes_command, Feature const& mouth_command) : impl_(std::make_shared<Impl>(lua, window, path, eyes_command, mouth_command))
{
}

auto Body::Expression(std::string const& expression, bool left_facing) -> double
{
  return impl_->Expression(expression, left_facing);
}

auto Body::Expression(bool left_facing) -> double
{
  return impl_->Expression(left_facing);
}

auto Body::Expression(std::string const& expression) -> double
{
  return impl_->Expression(expression);
}

auto Body::Next() -> double
{
  return impl_->Next();
}

auto Body::Render() const -> void
{
  impl_->Render();
}

auto Body::Position(game::Position const& position) -> void
{
  impl_->Position(position);
}

auto Body::Position() const -> game::Position
{
  return impl_->Position();
}

auto Body::Modulation(float r, float g, float b, float a) -> void
{
  impl_->Modulation(r, g, b, a);
}

auto Body::Modulation() const -> display::Modulation
{
  return impl_->Modulation();
}

auto Body::Period() const -> double
{
  return impl_->Period();
}

auto Body::Rotation(double angle) -> void
{
  impl_->Rotation(angle);
}
  
auto Body::Rotation() const -> double
{
  return impl_->Rotation();
}

Body::operator bool() const
{
  return bool(impl_);
}
}