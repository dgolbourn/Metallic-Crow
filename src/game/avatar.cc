#include "avatar.h"
#include "game_body.h"
#include "feature.h"
#include "timer.h"
#include "bind.h"
namespace game
{
class Avatar::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(json::JSON const& json, display::Window& window);
  void Init(event::Queue& queue);
  void Eyes(std::string const& expression);
  void Mouth(std::string const& expression);
  void Body(std::string const& expression);
  void Eyes(int open);
  void Mouth(int open);
  void Facing(bool left_facing);
  void ToggleFacing();
  void Position(game::Position const& position);
  void Modulation(display::Modulation const& modulation);
  void Pause(void);
  void Resume(void);
  void Render(void) const;
  void Next();

  game::Body body_;
  game::Feature eyes_;
  game::Feature mouth_;
  display::Modulation modulation_;
  game::Position position_;

  std::string body_expression_;
  std::string eyes_expression_;
  int eyes_open_;
  std::string mouth_expression_;
  int mouth_open_;
  bool facing_;

  event::Timer timer_;
};

Avatar::Impl::Impl(json::JSON const& json, display::Window& window) :
  eyes_open_(0),
  mouth_open_(0),
  facing_(false)
{
  double interval;
  json_t* body;
  json_t* eyes;
  json_t* mouth;

  json.Unpack("{sfsososo}",
    "frame period", &interval,
    "body", &body,
    "eyes", &eyes,
    "mouth", &mouth);

  timer_ = event::Timer(interval, -1);
  body_ = game::Body(json::JSON(body), window);
  eyes_ = game::Feature(json::JSON(eyes), window);
  mouth_ = game::Feature(json::JSON(mouth), window);
}

void Avatar::Impl::Init(event::Queue& queue)
{
  queue.Add(event::Bind(&event::Timer::operator(), timer_));
  timer_.Add(event::Bind(&Impl::Next, shared_from_this()));
  body_.Facing(event::Bind(&Impl::ToggleFacing, shared_from_this()));
}

void Avatar::Impl::Eyes(std::string const& expression)
{
  eyes_expression_ = expression;
  eyes_.Expression(eyes_expression_, eyes_open_, facing_);
}

void Avatar::Impl::Mouth(std::string const& expression)
{
  mouth_expression_ = expression;
  mouth_.Expression(mouth_expression_, mouth_open_, facing_);
}

void Avatar::Impl::Body(std::string const& expression)
{
  body_expression_ = expression;
  body_.Expression(body_expression_, facing_);
}

void Avatar::Impl::Eyes(int open)
{
  eyes_open_ = open;
  eyes_.Expression(eyes_expression_, eyes_open_, facing_);
}

void Avatar::Impl::Mouth(int open)
{
  mouth_open_ = open;
  mouth_.Expression(mouth_expression_, mouth_open_, facing_);
}

void Avatar::Impl::Facing(bool left_facing)
{
  facing_ = left_facing;
  body_.Expression(body_expression_, facing_);
  eyes_.Expression(eyes_expression_, eyes_open_, facing_);
  mouth_.Expression(mouth_expression_, mouth_open_, facing_);
}

void Avatar::Impl::ToggleFacing()
{
  facing_ ^= true;
  eyes_.Expression(eyes_expression_, eyes_open_, facing_);
  mouth_.Expression(mouth_expression_, mouth_open_, facing_);
}

void Avatar::Impl::Position(game::Position const& position)
{
  position_ = position;
}

void Avatar::Impl::Modulation(display::Modulation const& modulation)
{
  modulation_ = modulation;
}

void Avatar::Impl::Pause(void)
{
  timer_.Pause();
}

void Avatar::Impl::Resume(void)
{
  timer_.Resume();
}

void Avatar::Impl::Render(void) const
{
  body_.Render(position_, modulation_, false);
  if(auto optional = body_.Eyes())
  {
    game::Position position = *optional;
    position.first += position_.first;
    position.second += position_.second;
    eyes_.Render(position, modulation_);
  }
  if(auto optional = body_.Mouth())
  {
    game::Position position = *optional;
    position.first += position_.first;
    position.second += position_.second;
    mouth_.Render(position, modulation_);
  }
  body_.Render(position_, modulation_, true);
}

void Avatar::Impl::Next()
{
  body_.Next();
}

void Avatar::Eyes(std::string const& expression)
{
  impl_->Eyes(expression);
}

void Avatar::Mouth(std::string const& expression)
{
  impl_->Mouth(expression);
}

void Avatar::Body(std::string const& expression)
{
  impl_->Body(expression);
}

void Avatar::Mouth(int open)
{
  impl_->Mouth(open);
}

void Avatar::Eyes(int open)
{
  impl_->Eyes(open);
}

void Avatar::Facing(bool left_facing)
{
  impl_->Facing(left_facing);
}

void Avatar::Position(game::Position const& position)
{
  impl_->Position(position);
}

void Avatar::Modulation(display::Modulation const& modulation)
{
  impl_->Modulation(modulation);
}

void Avatar::Pause(void)
{
  impl_->Pause();
}

void Avatar::Resume(void)
{
  impl_->Resume();
}

void Avatar::Render(void) const
{
  impl_->Render();
}

Avatar::Avatar(json::JSON const& json, display::Window& window, event::Queue& queue) : impl_(std::make_shared<Impl>(json, window))
{
  impl_->Init(queue);
}
}