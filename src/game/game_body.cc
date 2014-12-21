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

Body::Impl::Impl(json::JSON const& json, display::Window& window, boost::filesystem::path const& path, Feature const& eyes_command, Feature const& mouth_command)
{
  json_t* expressions;
  char const* begin_expression;
  int begin_facing;
  double x, y;
  json_t* modulation;
  json.Unpack("{sosssbs[ff]so}",
    "expressions", &expressions,
    "expression", &begin_expression,
    "left facing", &begin_facing,
    "position", &x, &y,
    "modulation", &modulation);
 
  modulation_ = display::Modulation(json::JSON(modulation));

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
      json_t* jframe_facing;
      json_t* jeyes_box;
      json_t* jmouth_box;
      json_t* jtextures;
      json_t* jeyes_parallax;
      json_t* jmouth_parallax;
      json_t* jeyes_plane;
      json_t* jmouth_plane;
      frame.Unpack("{sosososososososo}",
        "left facing", &jframe_facing,
        "eyes box", &jeyes_box,
        "eyes parallax", &jeyes_parallax,
        "eyes plane", &jeyes_plane,
        "mouth box", &jmouth_box,
        "mouth parallax", &jmouth_parallax,
        "mouth plane", &jmouth_plane,
        "textures", &jtextures);

      if(eyes_command)
      {
        int ifacing;
        json::JSON(jframe_facing).Unpack("b", &ifacing);
        bool facing = (ifacing != 0);
        double dparallax;
        json::JSON(jeyes_parallax).Unpack("f", &dparallax);
        float eyes_parallax = float(dparallax);
        display::BoundingBox eyes((json::JSON(jeyes_box)));
        int iplane;
        json::JSON(jeyes_plane).Unpack("i", &iplane);
        frame_iter->boxes_.emplace_back(eyes, display::BoundingBox(eyes, display::BoundingBox()));
        frame_iter->scene_.Add([=](){return eyes_command(eyes, modulation_, eyes_parallax, facing);}, iplane);
      }

      if(mouth_command)
      {
        int ifacing;
        json::JSON(jframe_facing).Unpack("b", &ifacing);
        bool facing = (ifacing != 0);
        double dparallax;
        json::JSON(jmouth_parallax).Unpack("f", &dparallax);
        float mouth_parallax = float(dparallax);
        display::BoundingBox mouth((json::JSON(jmouth_box)));
        int iplane;
        json::JSON(jmouth_plane).Unpack("i", &iplane);
        frame_iter->boxes_.emplace_back(mouth, display::BoundingBox(mouth, display::BoundingBox()));
        frame_iter->scene_.Add([=](){return mouth_command(mouth, modulation_, mouth_parallax, facing); }, iplane);
      }

      for(json::JSON texture : json::JSON(jtextures))
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
      ++frame_iter;
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
    box.first.y(box.second.y() + position_.second);
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
          box.first.y(box.second.y() + position_.second);
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
    box.first.y(box.second.y() + position_.second);
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

void Body::Modulation(float r, float g, float b, float a)
{
  impl_->Modulation(r, g, b, a);
}

display::Modulation Body::Modulation() const
{
  return impl_->Modulation();
}

Body::operator bool() const
{
  return bool(impl_);
}
}