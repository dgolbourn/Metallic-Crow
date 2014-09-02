#include "game_body.h"
#include "animation.h"
#include <unordered_map>
#include "boost/functional/hash.hpp"
#include "json_iterator.h"
#include "signal.h"
namespace game
{
namespace
{
typedef std::vector<Position> Positions;
typedef std::pair<std::string, bool> NextExpression;

Positions MakePositions(json::JSON& json)
{
  Positions positions;
  if (json)
  {
    for (json::JSON const& value : json)
    {
      Position position;
      value.Unpack("[ff]", &position.first, &position.second);
      positions.push_back(position);
    }
  }
  return positions;
}

NextExpression MakeNextExpression(json::JSON const& json)
{
  NextExpression next("", false);
  if (json)
  {
    char const* next_expression;
    int next_facing;
    json.Unpack("{sssb}",
      "expression", &next_expression,
      "left facing", &next_facing);
    next = NextExpression(std::string(next_expression), (next_facing != 0));
  }
  return next;
}

struct Animation
{
  Animation(json::JSON const& back, json::JSON const& front, json::JSON& eyes, json::JSON& mouth, json::JSON const& render_box, json::JSON const& next, bool left_facing, display::Window& window);
  display::Animation back_;
  display::Animation front_;
  Positions eyes_;
  Positions mouth_;
  display::BoundingBox render_box_;
  NextExpression next_;
  bool left_facing_;
};

Animation::Animation(json::JSON const& back, json::JSON const& front, json::JSON& eyes, json::JSON& mouth, json::JSON const& render_box, json::JSON const& next, bool left_facing, display::Window& window) :
  back_(display::MakeAnimation(back, window)),
  front_(display::MakeAnimation(front, window)),
  eyes_(MakePositions(eyes)),
  mouth_(MakePositions(mouth)),
  render_box_(render_box),
  next_(MakeNextExpression(next)),
  left_facing_(left_facing)
{
}

struct Iterator
{
  display::Animation::iterator back_;
  display::Animation::iterator front_;
  Positions::iterator eyes_;
  Positions::iterator mouth_;
};

struct Texture
{
  display::Texture back_;
  display::Texture front_;
  Position eyes_;
  Position mouth_;
};

typedef std::pair<std::string, bool> Key;
typedef std::unordered_map<Key, Animation, boost::hash<Key>> AnimationMap;
typedef AnimationMap::iterator AnimationPtr;

template<class A, class B, class C> bool Increment(A& texture, B& iterator, C const& animation)
{
  bool end = true;
  auto end_iter = animation.end();
  if(iterator != end_iter)
  {
    ++iterator;
    if(iterator != end_iter)
    {
      texture = *iterator;
      end = false;
    }
  }
  return end;
}

template<class A, class B, class C> void Begin(A& texture, B& iterator, C& animation)
{
  iterator = animation.begin();
  if(iterator != animation.end())
  {
    texture = *iterator;
  }
}
}

class Body::Impl
{
public:
  Impl(json::JSON const& json, display::Window& window);
  void Expression(std::string const& expression, bool left_facing);
  void Next();
  void Reset();
  OptionalPosition Eyes() const;
  OptionalPosition Mouth() const;
  void Facing(event::Command const& command);
  void Render(Position const& position, display::Modulation const& modulation, bool front) const;
 
  AnimationMap animations_;
  AnimationPtr animation_;
  Iterator iterator_;
  Texture texture_;
  event::Signal facing_;
};

Body::Impl::Impl(json::JSON const& json, display::Window& window)
{
  json_t* expressions;
  json.Unpack("{so}",
    "expressions", &expressions);

  for(json::JSON const& value : json::JSON(expressions))
  {
    char const* expression;
    int facing;
    json_t* back;
    json_t* front;
    json_t* eyes;
    json_t* mouth;
    json_t* render_box;
    json_t* next;

    value.Unpack("{sssbsosososososo}", 
      "expression", &expression,
      "left facing", &facing,
      "back animation", &back,
      "front animation", &front,
      "eyes position", &eyes,
      "mouth position", &mouth,
      "render box", &render_box,
      "next", &next);

    bool left_facing = (facing != 0);
    animations_.emplace
    (
      Key(std::string(expression), left_facing),
      Animation(json::JSON(back), json::JSON(front), json::JSON(eyes), json::JSON(mouth), json::JSON(render_box), json::JSON(next), left_facing, window)
    );
  }
}

void Body::Impl::Expression(std::string const& expression, bool left_facing)
{
  auto temp = animations_.find(Key(expression, left_facing));
  if(temp != animations_.end())
  {
    animation_ = temp;
    Reset();
  }
}

void Body::Impl::Next()
{
  bool done = true;
  done &= Increment(texture_.eyes_, iterator_.eyes_, animation_->second.eyes_);
  done &= Increment(texture_.mouth_, iterator_.mouth_, animation_->second.mouth_);
  done &= Increment(texture_.back_, iterator_.back_, animation_->second.back_);
  done &= Increment(texture_.front_, iterator_.front_, animation_->second.front_);
  if(done)
  {
    if(animation_->second.next_.first != "")
    {
      if(animation_->second.left_facing_ != animation_->second.next_.second)
      {
        facing_();
      }
      Expression(animation_->second.next_.first, animation_->second.next_.second);
    }
  }
}

void Body::Impl::Reset()
{
  Begin(texture_.back_, iterator_.back_, animation_->second.back_);
  Begin(texture_.front_, iterator_.front_, animation_->second.front_);
  Begin(texture_.eyes_, iterator_.eyes_, animation_->second.eyes_);
  Begin(texture_.mouth_, iterator_.mouth_, animation_->second.mouth_);
}

Body::OptionalPosition Body::Impl::Eyes() const
{
  OptionalPosition position;
  if(!animation_->second.eyes_.empty())
  {
    position = texture_.eyes_;
  }
  return position;
}

Body::OptionalPosition Body::Impl::Mouth() const
{
  OptionalPosition position;
  if(!animation_->second.mouth_.empty())
  {
    position = texture_.mouth_;
  }
  return position;
}

void Body::Impl::Facing(event::Command const& command)
{
  facing_.Add(command);
}

void Body::Impl::Render(Position const& position, display::Modulation const& modulation, bool front) const
{
  display::BoundingBox box = animation_->second.render_box_.Copy();
  box.x(box.x() + position.first);
  box.y(box.y() + position.second);
  
  display::Texture texture;
  if(front)
  {
    texture = texture_.front_;
  }
  else
  {
    texture = texture_.back_;
  }
  
  texture(display::BoundingBox(), box, 1.f, false, 0., modulation);
}

Body::Body(json::JSON const& json, display::Window& window) : impl_(std::make_shared<Impl>(json, window))
{
}

void Body::Expression(std::string const& expression, bool left_facing)
{
  impl_->Expression(expression, left_facing);
}

void Body::Next()
{
  impl_->Next();
}

Body::OptionalPosition Body::Eyes() const
{
  return impl_->Eyes();
}

Body::OptionalPosition Body::Mouth() const
{
  return impl_->Mouth();
}

void Body::Facing(event::Command const& command)
{
  return impl_->Facing(command);
}

void Body::Render(Position const& position, display::Modulation const& modulation, bool front) const
{
  impl_->Render(position, modulation, front);
}
}