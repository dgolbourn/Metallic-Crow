#include "game_body.h"
#include <unordered_map>
#include <map>
#include <vector>
#include <list>
#include "boost/functional/hash.hpp"
#include "json_iterator.h"
#include "scene.h"
namespace
{
struct Frame
{
  game::Scene scene_;
  std::vector<std::pair<display::BoundingBox, display::BoundingBox>> boxes_;
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
  Impl(json::JSON const& json, display::Window& window, boost::filesystem::path const& path, Feature const& eyes, Feature const& mouth);
  void Expression(std::string const& expression, bool left_facing);
  void Expression(std::string const& expression);
  void Expression(bool left_facing);
  void Expression();
  void Next();
  void Position(game::Position const& position);
  game::Position Position() const;
  void Modulation(display::Modulation const& modulation);
  void Render();
 
  Frames::Map expressions_;
  Frames::Map::iterator current_frames_;
  std::vector<Frame>::iterator current_frame_;
  Frames::Map::iterator next_;
  game::Position position_;
  display::Modulation modulation_;
  Frames::Key state_;
};

Body::Impl::Impl(json::JSON const& json, display::Window& window, boost::filesystem::path const& path, Feature const& eyes_command, Feature const& mouth_command) : modulation_(1.f, 1.f, 1.f, 1.f)
{
  json_t* expressions;
  char const* begin_expression;
  int begin_facing;
  double x, y;
  json.Unpack("{sosssbs[ff]}",
    "expressions", &expressions,
    "expression", &begin_expression,
    "left facing", &begin_facing,
    "position", &x, &y);

  std::map<Frames::Key, std::list<Frames::Map::iterator*>> next;

  for(json::JSON expression : json::JSON(expressions))
  {
    char const* name;
    char const* next_expression;
    int facing;
    int next_facing;
    int interruptable;
    json_t* frames_ref;

    expression.Unpack("{sssbsssbsbso}", 
      "name", &name,
      "left facing", &facing,
      "next name", &next_expression,
      "next left facing", &next_facing,
      "interruptable", &interruptable,
      "frames", &frames_ref);

    Frames& frames = expressions_[Frames::Key(name, facing != 0)];
    frames.iterruptable_ = (interruptable != 0);

    next[Frames::Key(next_expression, (next_facing != 0))].push_back(&frames.next_);

    frames.frames_.resize(json::JSON(frames_ref).Size());
    auto frame_iter = frames.frames_.begin();
    for(json::JSON frame : json::JSON(frames_ref))
    {
      int frame_facing;
      json_t* eyes_box;
      json_t* mouth_box;
      json_t* textures;
      double deyes_parallax;
      double dmouth_parallax;
      int eyes_plane;
      int mouth_plane;
      frame.Unpack("{sbsosfsisosfsiso}"
        "left facing", &frame_facing,
        "eyes box", &eyes_box,
        "eyes parallax", &deyes_parallax,
        "eyes plane", &eyes_plane,
        "mouth box", &mouth_box,
        "mouth parallax", &dmouth_parallax,
        "mouth plane", &mouth_plane,
        "textures", &textures);

      bool facing = (frame_facing != 0);
           
      if(eyes_command)
      {
        float eyes_parallax = float(deyes_parallax);
        display::BoundingBox eyes((json::JSON(eyes_box)));
        frame_iter->boxes_.emplace_back(eyes, display::BoundingBox(eyes, display::BoundingBox()));
        frame_iter->scene_.Add([=](){return eyes_command(eyes, modulation_, eyes_parallax, facing);}, eyes_plane);
      }

      if(mouth_command)
      {
        float mouth_parallax = float(dmouth_parallax);
        display::BoundingBox mouth((json::JSON(mouth_box)));
        frame_iter->boxes_.emplace_back(mouth, display::BoundingBox(mouth, display::BoundingBox()));
        frame_iter->scene_.Add([=](){return mouth_command(mouth, modulation_, mouth_parallax, facing);}, mouth_plane);
      }

      for(json::JSON texture : json::JSON(textures))
      {
        char const* page;
        json_t* clip;
        json_t* render_box;
        int plane;
        double dparallax;
        texture.Unpack("{sssososfsi}",
          "page", &page,
          "clip", &clip,
          "render box", &render_box,
          "parallax", &dparallax,
          "plane", &plane);

        display::BoundingBox render((json::JSON(render_box)));
        frame_iter->boxes_.emplace_back(render, display::BoundingBox(render, display::BoundingBox()));

        float parallax = float(dparallax);
        display::Texture texture(display::Texture(path / page, window), display::BoundingBox(json::JSON(clip)));
        frame_iter->scene_.Add([=](){return texture(display::BoundingBox(), render, parallax, false, 0., modulation_);}, plane);
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

  state_ = Frames::Key(begin_expression, begin_facing != 0);
  current_frames_ = expressions_.find(state_);
  current_frame_ = current_frames_->second.frames_.begin();
  next_ = expressions_.end();
  position_.first = float(x);
  position_.second = float(y);
  for(auto& box : current_frame_->boxes_)
  {
    box.first.x(box.second.x() + position_.first);
    box.first.y(box.second.y() + position_.first);
  }
}

void Body::Impl::Expression(std::string const& expression, bool left_facing)
{
  state_.first = expression;
  state_.second = left_facing;
  Expression();
}

void Body::Impl::Expression(std::string const& expression)
{
  state_.first = expression;
  Expression();
}

void Body::Impl::Expression(bool left_facing)
{
  state_.second = left_facing;
  Expression();
}

void Body::Impl::Expression()
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
          box.first.y(box.second.y() + position_.first);
        }
      }
      else
      {
        next_ = next;
      }
    }
  }
}

void Body::Impl::Next()
{
  ++current_frame_;
  if(current_frame_ == current_frames_->second.frames_.end())
  {
    if(current_frames_->second.iterruptable_ && (next_ != expressions_.end()))
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
    box.first.y(box.second.y() + position_.first);
  }
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
    box.first.y(box.second.y() + position_.first);
  }
}

game::Position Body::Impl::Position() const
{
  return position_;
}

void Body::Impl::Modulation(display::Modulation const& modulation)
{
  modulation_.r(modulation.r());
  modulation_.g(modulation.g());
  modulation_.b(modulation.b());
  modulation_.a(modulation.a());
}

Body::Body(json::JSON const& json, display::Window& window, boost::filesystem::path const& path, Feature const& eyes_command, Feature const& mouth_command) : impl_(std::make_shared<Impl>(json, window, path, eyes_command, mouth_command))
{
}

void Body::Expression(std::string const& expression, bool left_facing)
{
  impl_->Expression(expression, left_facing);
}

void Body::Expression(bool left_facing)
{
  impl_->Expression(left_facing);
}

void Body::Expression(std::string const& expression)
{
  impl_->Expression(expression);
}

void Body::Next()
{
  impl_->Next();
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

void Body::Modulation(display::Modulation const& modulation)
{
  impl_->Modulation(modulation);
}

Body::operator bool() const
{
  return bool(impl_);
}
}