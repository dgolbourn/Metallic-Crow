#include "event.h"
#include "SDL.h"
#include "SDL_events.h"
#include "SDL_scancode.h"
#include "signal.h"
#include "sdl_library.h"
#include <map>
#include <cmath>
#include "sdl_exception.h"
#include "log.h"
namespace
{
typedef std::list<event::Event::Command> Commands;

struct ControllerState
{
  SDL_GameController* controller_;
  float x_raw_;
  float y_raw_;
  float x_report_;
  float y_report_;
};

typedef std::map<Sint32, ControllerState> ControllerStateMap;

struct ControllerTemp
{
  Sint32 x_;
  Sint32 y_;
  int n_;
  int m_;
};

typedef std::map<Sint32, ControllerTemp> ControllerTempMap;
}

namespace event
{
class Event::Impl
{
public:
  Impl(json::JSON const& json);
  ~Impl();
  void Check();
  void Control(Command const& command);
  void ChoiceUp(event::Command const& command);
  void ChoiceDown(event::Command const& command);
  void ChoiceLeft(event::Command const& command);
  void ChoiceRight(event::Command const& command);
  void Select(event::Command const& command);
  void Back(event::Command const& command);
  void Quit(event::Command const& command);
  void Destroy() noexcept;
  sdl::Library sdl_;
  Commands report_;
  Signal choice_up_;
  Signal choice_down_;
  Signal choice_left_;
  Signal choice_right_;
  Signal quit_;
  Signal back_;
  Signal select_;
  SDL_Scancode key_up_;
  SDL_Scancode key_down_;
  SDL_Scancode key_left_;
  SDL_Scancode key_right_;
  SDL_Scancode key_choice_up_;
  SDL_Scancode key_choice_down_;
  SDL_Scancode key_choice_left_;
  SDL_Scancode key_choice_right_;
  SDL_Scancode key_quit_;
  SDL_Scancode key_back_;
  SDL_Scancode key_select_;
  ControllerStateMap controllers_;
  float update_offset_;
  float update_scale_;
  float update_threshold_;
  float x_report_;
  float y_report_;
  float x_key_;
  float y_key_;
  bool key_state_up_;
  bool key_state_down_;
  bool key_state_left_;
  bool key_state_right_;
};

void Event::Impl::Destroy() noexcept
{
  try
  {
    for(auto& controller : controllers_)
    {
      SDL_GameControllerClose(controller.second.controller_);
    }
  }
  catch(...)
  {
    exception::Log("Swallowed exception");
  }
}

Event::Impl::~Impl()
{
  Destroy();
}

void Event::Impl::Control(Command const& command)
{
  report_.push_back(command);
}

void Event::Impl::ChoiceUp(event::Command const& command)
{
  choice_up_.Add(command);
}

void Event::Impl::ChoiceDown(event::Command const& command)
{
  choice_down_.Add(command);
}

void Event::Impl::ChoiceLeft(event::Command const& command)
{
  choice_left_.Add(command);
}

void Event::Impl::ChoiceRight(event::Command const& command)
{
  choice_right_.Add(command);
}

void Event::Impl::Select(event::Command const& command)
{
  select_.Add(command);
}

void Event::Impl::Back(event::Command const& command)
{
  back_.Add(command);
}

void Event::Impl::Quit(event::Command const& command)
{
  quit_.Add(command);
}

Event::Impl::Impl(json::JSON const& json) : sdl_(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER), x_report_(0.f), y_report_(0.f), x_key_(0.f), y_key_(0.f), key_state_up_(false), key_state_down_(false), key_state_left_(false), key_state_right_(false)
{
  try
  {
    int up, down, left, right, choice_up, choice_down, choice_left, choice_right, select, back;
    double max, min, threshold;
    json.Unpack("{sisisisisisisisisisisfsfsf}",
      "up", &up,
      "down", &down,
      "left", &left,
      "right", &right,
      "choice up", &choice_up,
      "choice down", &choice_down,
      "choice left", &choice_left,
      "choice right", &choice_right,
      "select", &select,
      "back", &back,
      "update max", &max,
      "update min", &min,
      "update threshold", &threshold);

    key_up_ = SDL_Scancode(up);
    key_down_ = SDL_Scancode(down);
    key_left_ = SDL_Scancode(left);
    key_right_ = SDL_Scancode(right);
    key_choice_up_ = SDL_Scancode(choice_up);
    key_choice_down_ = SDL_Scancode(choice_down);
    key_choice_left_ = SDL_Scancode(choice_left);
    key_choice_right_ = SDL_Scancode(choice_right);
    key_select_ = SDL_Scancode(select);
    key_back_ = SDL_Scancode(back);

    for(int i = 0; i < SDL_NumJoysticks(); ++i)
    {
      if(SDL_IsGameController(i))
      {
        if(SDL_GameController* game_controller = SDL_GameControllerOpen(i))
        {
          int joystick = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(game_controller));
          if(joystick == -1)
          {
            BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
          }
          controllers_.emplace(joystick, ControllerState{game_controller, 0.f, 0.f, 0.f, 0.f});
        }
        else
        {
          BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
        }
      }
    }

    update_offset_ = -float(min);
    update_scale_ = float(1. / (max - min));
    update_threshold_ = float(threshold * threshold);
  }
  catch(...)
  {
    Destroy();
    throw;
  }
}

void Event::Impl::Check()
{
  ControllerTempMap controllers;
  bool key = false;
  bool key_choice_up = false;
  bool key_choice_down = false;
  bool key_choice_left = false;
  bool key_choice_right = false;
  bool key_quit = false;
  bool key_back = false;
  bool key_select = false;

  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
    case SDL_QUIT:
      quit_();
      break;
    case SDL_KEYDOWN:
      if(event.key.repeat == 0)
      {
        SDL_Scancode code = event.key.keysym.scancode;
        if(code == key_up_)
        {
          key_state_up_ = true;
          key = true;
        }
        else if(code == key_down_)
        {
          key_state_down_ = true;
          key = true;
        }
        else if(code == key_left_)
        {
          key_state_left_ = true;
          key = true;
        }
        else if(code == key_right_)
        {
          key_state_right_ = true;
          key = true;
        }
        else if(code == key_choice_up_)
        {
          key_choice_up = true;
        }
        else if(code == key_choice_down_)
        {
          key_choice_down = true;
        }
        else if(code == key_choice_left_)
        {
          key_choice_left = true;
        }
        else if(code == key_choice_right_)
        {
          key_choice_right = true;
        }
        else if(code == key_select_)
        {
          key_select = true;
        }
        else if(code == key_back_)
        {
          key_back = true;
        }
      }
      break;
    case SDL_KEYUP:
      if(event.key.repeat == 0)
      {
        SDL_Scancode code = event.key.keysym.scancode;
        if(code == key_up_)
        {
          key_state_up_ = false;
          key = true;
        }
        else if(code == key_down_)
        {
          key_state_down_ = false;
          key = true;
        }
        else if(code == key_left_)
        {
          key_state_left_ = false;
          key = true;
        }
        else if(code == key_right_)
        {
          key_state_right_ = false;
          key = true;
        }
      }
      break;
    case SDL_CONTROLLERDEVICEADDED:
      if(SDL_GameController* game_controller = SDL_GameControllerOpen(event.cdevice.which))
      {
        int joystick = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(game_controller));
        if(joystick == -1)
        {
          BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
        }
        controllers_.emplace(joystick, ControllerState{game_controller, 0.f, 0.f, 0.f, 0.f});
      }
      else
      {
        BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
      }
      break;
    case SDL_CONTROLLERDEVICEREMOVED:
      {
        auto iter = controllers_.find(event.cdevice.which);
        if(iter != controllers_.end())
        {
          SDL_GameControllerClose(iter->second.controller_);
          controllers_.erase(iter);
        }
      }
      break;
    case SDL_CONTROLLERBUTTONDOWN:
      switch(event.cbutton.button)
      {
      case SDL_CONTROLLER_BUTTON_A:
        key_choice_down = true;
        break;
      case SDL_CONTROLLER_BUTTON_B:
        key_choice_right = true;
        break;
      case SDL_CONTROLLER_BUTTON_X:
        key_choice_left = true;
        break;
      case SDL_CONTROLLER_BUTTON_Y:
        key_choice_up = true;
        break;
      case SDL_CONTROLLER_BUTTON_BACK:
        key_back = true;
        break;
      case SDL_CONTROLLER_BUTTON_START:
        key_select = true;
        break;
      default:
        break;
      }
      break;
    case SDL_CONTROLLERAXISMOTION:
      switch(event.caxis.axis)
      {
      case SDL_CONTROLLER_AXIS_LEFTX:
        if(controllers_.find(event.cdevice.which) != controllers_.end())
        {
          auto iter = controllers.find(event.cdevice.which);
          if(iter == controllers.end())
          {
            iter = controllers.emplace(event.cdevice.which, ControllerTemp{0, 0, 0, 0}).first;
          }
          iter->second.x_ += event.caxis.value;
          ++(iter->second.m_);
        }
        break;
      case SDL_CONTROLLER_AXIS_LEFTY:
        if(controllers_.find(event.cdevice.which) != controllers_.end())
        {
          auto iter = controllers.find(event.cdevice.which);
          if(iter == controllers.end())
          {
            iter = controllers.emplace(event.cdevice.which, ControllerTemp{0, 0, 0, 0}).first;
          }
          iter->second.y_ += event.caxis.value;
          ++(iter->second.n_);
        }
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }

  if(key_choice_up)
  {
    choice_up_();
  }
  else if(key_choice_down)
  {
    choice_down_();
  }
  else if(key_choice_left)
  {
    choice_left_();
  }
  else if(key_choice_right)
  {
    choice_right_();
  }
  else if(key_select)
  {
    select_();
  }
  else if(key_back)
  {
    back_();
  }

  for(auto& controller : controllers)
  {
    auto state = controllers_.find(controller.first);
    if(state != controllers_.end())
    {
      if((controller.second.m_) || (controller.second.n_))
      {
        if(controller.second.m_)
        {
          state->second.x_raw_ = float(controller.second.x_) / controller.second.m_;
        }
        if(controller.second.n_)
        {
          state->second.y_raw_ = float(controller.second.y_) / controller.second.n_;
        }

        float angle = std::atan2(state->second.y_raw_, state->second.x_raw_);
        float length = std::sqrt(state->second.x_raw_ * state->second.x_raw_ + state->second.y_raw_ * state->second.y_raw_) / INT16_MAX;
        length += update_offset_;
        length *= update_scale_;
        length = std::min(std::max(length, 0.f), 1.f);
        
        state->second.x_report_ = length * std::cos(angle);        
        state->second.y_report_ = -length * std::sin(angle);
      }
    }
  }

  float x_report = 0.f;
  float y_report = 0.f;
  bool update = false;

  if(!controllers_.empty())
  {
    int n = 0;
    for(auto& controller : controllers_)
    {
      if((controller.second.x_report_ != 0.f) || (controller.second.y_report_ != 0.f))
      {
        x_report += controller.second.x_report_;
        y_report += controller.second.y_report_;
        ++n;
      }
      else if(controllers.find(controller.first) != controllers.end())
      {
        ++n;
      }
    }
    if(n)
    {
      float scale = 1.f / n;
      x_report *= scale;
      y_report *= scale;
      update = true;
    }
  }

  if(key)
  {
    float length;
    if((key_state_left_ != key_state_right_) && (key_state_up_ != key_state_down_))
    {
      length = std::sqrt(.5f);
    }
    else
    {
      length = 1.f;
    }

    if(key_state_left_ != key_state_right_)
    {
      if(key_state_right_)
      {
        x_key_ = length;
      }
      else
      {
        x_key_ = -length;
      }
    }
    else
    {
      x_key_ = 0.f;
    }

    if(key_state_up_ != key_state_down_)
    {
      if(key_state_up_)
      {
        y_key_ = length;
      }
      else
      {
        y_key_ = -length;
      }
    }
    else
    {
      y_key_ = 0.f;
    }

    x_report += x_key_;
    y_report += y_key_;
    update = true;
  }
  else if(key_state_up_ || key_state_down_ || key_state_left_ || key_state_right_)
  {
    x_report += x_key_;
    y_report += y_key_;
    update = true;
  }

  if(update)
  { 
    float dx = x_report_ - x_report;
    float dy = y_report_ - y_report;
    if((dx * dx + dy * dy) > update_threshold_)
    {
      x_report_ = x_report;
      y_report_ = y_report;
  
      for(auto iter = report_.begin(); iter != report_.end();)
      {
        if((*iter)(x_report_, y_report_))
        {
          ++iter;
        }
        else
        {
          iter = report_.erase(iter);
        }
      }
    }
  }
}

Event::Event(json::JSON const& json) : impl_(std::make_shared<Impl>(json))
{
}

void Event::operator()()
{
  impl_->Check();
}

void Event::Control(Command const& command)
{
  impl_->Control(command);
}

void Event::ChoiceUp(event::Command const& command)
{
  impl_->ChoiceUp(command);
}

void Event::ChoiceDown(event::Command const& command)
{
  impl_->ChoiceDown(command);
}

void Event::ChoiceLeft(event::Command const& command)
{
  impl_->ChoiceLeft(command);
}

void Event::ChoiceRight(event::Command const& command)
{
  impl_->ChoiceRight(command);
}

void Event::Select(event::Command const& command)
{
  impl_->Select(command);
}

void Event::Back(event::Command const& command)
{
  impl_->Back(command);
}

void Event::Quit(event::Command const& command)
{
  impl_->Quit(command);
}

Event::operator bool() const
{
  return bool(impl_);
}
}