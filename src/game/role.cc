#include "role.h"
#include "timer.h"
#include <cmath>

namespace game
{
class RoleImpl
{
public:
  RoleImpl(void);
  bool Decision(void);
  PositionCommand hero_position_;
  PositionCommand position_;
  event::Command up_;
  event::Command down_;
  event::Command left_;
  event::Command right_;
  int x_sign_;
  int y_sign_;
  event::Timer timer_;
};

RoleImpl::RoleImpl(void)
{
  //timer_ = event::Timer(500);
  //timer_.Add(std::bind(&RoleImpl::Decision, this));
}

bool RoleImpl::Decision(void)
{
  Position hero = hero_position_();
  Position position = position_();

  float dx = hero.first - position.first;
  float dy = hero.second - position.second;
  
  if(dx > 0.f)
  {
    left_();
  }
  else if (dx < 0.f)
  {
    right_();
  }

  return true;
}

void Role::Up(event::Command const& command)
{
}

void Role::Down(event::Command const& command)
{
}

void Role::Left(event::Command const& command)
{
}

void Role::Right(event::Command const& command)
{
}

void Role::Attack(event::Command const& command)
{
}

void Role::HeroPosition(PositionCommand const& position)
{
}

void Role::Position(PositionCommand const& position)
{
}

Role::Role(void)
{
}
}