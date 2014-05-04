#include "Box.h"
#include "bounding_box.h"
#include "event.h"
#include "body.h"
#include "bind.h"
#include "state.h"
namespace game
{
class BoxImpl final : public std::enable_shared_from_this<BoxImpl>
{
public:
  BoxImpl(json::JSON const& json, display::Window& window, event::Queue& queue, DynamicsCollision& dcollision, dynamics::World& world);
  void Init(dynamics::World& world, Scene& scene);
  State state_;
  display::BoundingBox render_box_;
  bool paused_;
  dynamics::Body body_;
  void Pause(void);
  void Resume(void);
  void Position(game::Position const& position);
  game::Position Position(void) const;
  void Update(void);
  void Render(void) const;
};

void BoxImpl::Pause(void)
{
  paused_ = true;
  state_.Pause();
}

void BoxImpl::Resume(void)
{
  paused_ = false;
  state_.Resume();
}

void BoxImpl::Update(void)
{
  game::Position position = Position();
  display::BoundingBox temp = state_.Shape().Copy();
  temp.x(temp.x() + position.first);
  temp.y(temp.y() + position.second);
  render_box_.Copy(temp);
}

void BoxImpl::Render(void) const
{
  state_.Render(render_box_);
}

BoxImpl::BoxImpl(json::JSON const& json, display::Window& window, event::Queue& queue, DynamicsCollision& dcollision, dynamics::World& world)
{
  json_t* state;
  json_t* body;

  json.Unpack("{soso}",
    "state", &state,
    "body", &body);
  
  paused_ = true;
  state_ = State(state, window, queue);
  state_.Play();
  state_.Pause();
  body_ = dynamics::Body(body, world);
  render_box_ = display::BoundingBox(0.f, 0.f, 0.f, 0.f);
  dcollision.Add(dynamics::Type::Body, body_);
  Update();
}

void BoxImpl::Init(dynamics::World& world, Scene& scene)
{
  auto ptr = shared_from_this();
  world.Add(event::Bind(&BoxImpl::Update, ptr));
  scene.Add(event::Bind(&BoxImpl::Render, ptr), -1);
}

void BoxImpl::Position(game::Position const& position)
{
  body_.Position(position.first, position.second);
}

game::Position BoxImpl::Position(void) const
{
  return body_.Position();
}

void Box::Position(game::Position const& position)
{
  impl_->Position(position);
}

game::Position Box::Position(void) const
{
  return impl_->Position();
}

void Box::Pause(void)
{
  impl_->Pause();
}

void Box::Resume(void)
{
  impl_->Resume();
}

Box::Box(json::JSON const& json, display::Window& window, Scene& scene, event::Queue& queue, DynamicsCollision& dcollision, dynamics::World& world)
{
  impl_ = std::make_shared<BoxImpl>(json, window, queue, dcollision, world);
  impl_->Init(world, scene);
}
}