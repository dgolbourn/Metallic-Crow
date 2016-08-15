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
#include "lua_exception.h"
namespace
{
typedef std::list<event::Event::Command> Commands;

struct ControllerState
{
  SDL_GameController* controller_;
  float x_move_raw_;
  float y_move_raw_;
  float x_move_report_;
  float y_move_report_;
  float x_look_raw_;
  float y_look_raw_;
  float x_look_report_;
  float y_look_report_;
};

typedef std::map<Sint32, ControllerState> ControllerStateMap;

struct ControllerTemp
{
  Sint32 x_move_;
  Sint32 y_move_;
  int n_move_;
  int m_move_;
  Sint32 x_look_;
  Sint32 y_look_;
  int n_look_;
  int m_look_;
};

typedef std::map<Sint32, ControllerTemp> ControllerTempMap;

auto GetScanCodeField(lua::Stack& lua, std::string const& field) -> SDL_Scancode 
{
  return static_cast<SDL_Scancode>(lua.Field<int>(field));
}
}

namespace event
{
class Event::Impl
{
public:
  Impl(lua::Stack& lua);
  ~Impl();
  auto Check() ->void;
  auto Control(Command const& command) -> void;
  auto Look(Command const& command) -> void;
  auto ChoiceUp(event::Command const& command) -> void;
  auto ChoiceDown(event::Command const& command) -> void;
  auto ChoiceLeft(event::Command const& command) -> void;
  auto ChoiceRight(event::Command const& command) -> void;
  auto ActionLeft(event::Command const& command) -> void;
  auto ActionRight(event::Command const& command) -> void;
  auto Select(event::Command const& command) -> void;
  auto Back(event::Command const& command) -> void;
  auto Quit(event::Command const& command) -> void;
  auto Destroy() noexcept -> void;
  sdl::Library sdl_;
  Commands move_report_;
  Commands look_report_;
  Signal choice_up_;
  Signal choice_down_;
  Signal choice_left_;
  Signal choice_right_;
  Signal quit_;
  Signal back_;
  Signal select_;
  Signal action_left_;
  Signal action_right_;
  SDL_Scancode key_move_up_;
  SDL_Scancode key_move_down_;
  SDL_Scancode key_move_left_;
  SDL_Scancode key_move_right_;
  SDL_Scancode key_look_up_;
  SDL_Scancode key_look_down_;
  SDL_Scancode key_look_left_;
  SDL_Scancode key_look_right_;
  SDL_Scancode key_choice_up_;
  SDL_Scancode key_choice_down_;
  SDL_Scancode key_choice_left_;
  SDL_Scancode key_choice_right_;
  SDL_Scancode key_quit_;
  SDL_Scancode key_back_;
  SDL_Scancode key_select_;
  SDL_Scancode key_action_left_;
  SDL_Scancode key_action_right_;
  ControllerStateMap controllers_;
  float update_offset_;
  float update_scale_;
  float update_threshold_;
  float x_move_report_;
  float y_move_report_;
  float x_move_key_;
  float y_move_key_;
  float x_look_report_;
  float y_look_report_;
  float x_look_key_;
  float y_look_key_;
  bool key_move_state_up_;
  bool key_move_state_down_;
  bool key_move_state_left_;
  bool key_move_state_right_;
  bool key_look_state_up_;
  bool key_look_state_down_;
  bool key_look_state_left_;
  bool key_look_state_right_;
};

auto Event::Impl::Destroy() noexcept -> void
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

auto Event::Impl::Control(Command const& command) -> void
{
  move_report_.push_back(command);
}

auto Event::Impl::Look(Command const& command) -> void
{
  look_report_.push_back(command);
}

auto Event::Impl::ChoiceUp(event::Command const& command) -> void
{
  choice_up_.Add(command);
}

auto Event::Impl::ChoiceDown(event::Command const& command) -> void
{
  choice_down_.Add(command);
}

auto Event::Impl::ChoiceLeft(event::Command const& command) -> void
{
  choice_left_.Add(command);
}

auto Event::Impl::ChoiceRight(event::Command const& command) -> void
{
  choice_right_.Add(command);
}

auto Event::Impl::ActionLeft(event::Command const& command) -> void
{
  action_left_.Add(command);
}

auto Event::Impl::ActionRight(event::Command const& command) -> void
{
  action_right_.Add(command);
}

auto Event::Impl::Select(event::Command const& command) -> void
{
  select_.Add(command);
}

auto Event::Impl::Back(event::Command const& command) -> void
{
  back_.Add(command);
}

auto Event::Impl::Quit(event::Command const& command) -> void
{
  quit_.Add(command);
}

Event::Impl::Impl(lua::Stack& lua) : sdl_(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER), x_move_report_(0.f), y_move_report_(0.f), x_move_key_(0.f), y_move_key_(0.f), x_look_report_(0.f), y_look_report_(0.f), x_look_key_(0.f), y_look_key_(0.f), key_move_state_up_(false), key_move_state_down_(false), key_move_state_left_(false), key_move_state_right_(false), key_look_state_up_(false), key_look_state_down_(false), key_look_state_left_(false), key_look_state_right_(false)
{
  try
  {
    key_move_up_ = GetScanCodeField(lua, "move_up");
    key_move_down_ = GetScanCodeField(lua, "move_down");
    key_move_left_ = GetScanCodeField(lua, "move_left");
    key_move_right_ = GetScanCodeField(lua, "move_right");
    key_look_up_ = GetScanCodeField(lua, "look_up");
    key_look_down_ = GetScanCodeField(lua, "look_down");
    key_look_left_ = GetScanCodeField(lua, "look_left");
    key_look_right_ = GetScanCodeField(lua, "look_right");
    key_choice_up_ = GetScanCodeField(lua, "choice_up");
    key_choice_down_ = GetScanCodeField(lua, "choice_down");
    key_choice_left_ = GetScanCodeField(lua, "choice_left");
    key_choice_right_ = GetScanCodeField(lua, "choice_right");
    key_action_left_ = GetScanCodeField(lua, "action_left");
    key_action_right_ = GetScanCodeField(lua, "action_right");
    key_select_ = GetScanCodeField(lua, "select");
    key_back_ = GetScanCodeField(lua, "back");

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
          controllers_.emplace(joystick, ControllerState{ game_controller, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f });
        }
        else
        {
          BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
        }
      }
    }

    double max = lua.Field<double>("update_max");
    double min = lua.Field<double>("update_min");
    double threshold = lua.Field<double>("update_threshold");

    update_offset_ = -static_cast<float>(min);
    update_scale_ = static_cast<float>(1. / (max - min));
    update_threshold_ = static_cast<float>(threshold * threshold);
  }
  catch(...)
  {
    Destroy();
    throw;
  }
}

auto Event::Impl::Check() -> void
{
  ControllerTempMap controllers;
  bool key_look = false;
  bool key_move = false;
  bool key_choice_up = false;
  bool key_choice_down = false;
  bool key_choice_left = false;
  bool key_choice_right = false;
  bool key_quit = false;
  bool key_back = false;
  bool key_select = false;
  bool key_action_left = false;
  bool key_action_right = false;

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
        if(code == key_move_up_)
        {
          key_move_state_up_ = true;
          key_move = true;
        }
        else if(code == key_move_down_)
        {
          key_move_state_down_ = true;
          key_move = true;
        }
        else if(code == key_move_left_)
        {
          key_move_state_left_ = true;
          key_move = true;
        }
        else if(code == key_move_right_)
        {
          key_move_state_right_ = true;
          key_move = true;
        }
        else if(code == key_look_up_)
        {
          key_look_state_up_ = true;
          key_look = true;
        }
        else if(code == key_look_down_)
        {
          key_look_state_down_ = true;
          key_look = true;
        }
        else if(code == key_look_left_)
        {
          key_look_state_left_ = true;
          key_look = true;
        }
        else if(code == key_look_right_)
        {
          key_look_state_right_ = true;
          key_look = true;
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
        else if(code == key_action_left_)
        {
          key_action_left = true;
        }
        else if(code == key_action_right_)
        {
          key_action_right = true;
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
        if(code == key_move_up_)
        {
          key_move_state_up_ = false;
          key_move = true;
        }
        else if(code == key_move_down_)
        {
          key_move_state_down_ = false;
          key_move = true;
        }
        else if(code == key_move_left_)
        {
          key_move_state_left_ = false;
          key_move = true;
        }
        else if(code == key_move_right_)
        {
          key_move_state_right_ = false;
          key_move = true;
        }
        else if(code == key_look_up_)
        {
          key_look_state_up_ = false;
          key_look = true;
        }
        else if(code == key_look_down_)
        {
          key_look_state_down_ = false;
          key_look = true;
        }
        else if(code == key_look_left_)
        {
          key_look_state_left_ = false;
          key_look = true;
        }
        else if(code == key_look_right_)
        {
          key_look_state_right_ = false;
          key_look = true;
        }       
        else if(code == key_action_left_)
        {
          key_action_left = true;
        }
        else if(code == key_action_right_)
        {
          key_action_right = true;
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
        controllers_.emplace(joystick, ControllerState{ game_controller, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f });
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
      case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        key_action_left = true;
        break;
      case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        key_action_right = true;
        break;
      default:
        break;
      }
      break;
    case SDL_CONTROLLERBUTTONUP:
      switch(event.cbutton.button)
      {
      case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        key_action_left = true;
        break;
      case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        key_action_right = true;
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
            iter = controllers.emplace(event.cdevice.which, ControllerTemp{0, 0, 0, 0, 0, 0, 0, 0}).first;
          }
          iter->second.x_move_ += event.caxis.value;
          ++(iter->second.m_move_);
        }
        break;
      case SDL_CONTROLLER_AXIS_LEFTY:
        if(controllers_.find(event.cdevice.which) != controllers_.end())
        {
          auto iter = controllers.find(event.cdevice.which);
          if(iter == controllers.end())
          {
            iter = controllers.emplace(event.cdevice.which, ControllerTemp{0, 0, 0, 0, 0, 0, 0, 0}).first;
          }
          iter->second.y_move_ += event.caxis.value;
          ++(iter->second.n_move_);
        }
        break;
      case SDL_CONTROLLER_AXIS_RIGHTX:
        if(controllers_.find(event.cdevice.which) != controllers_.end())
        {
          auto iter = controllers.find(event.cdevice.which);
          if(iter == controllers.end())
          {
            iter = controllers.emplace(event.cdevice.which, ControllerTemp{0, 0, 0, 0, 0, 0, 0, 0}).first;
          }
          iter->second.x_look_ += event.caxis.value;
          ++(iter->second.m_look_);
        }
        break;
      case SDL_CONTROLLER_AXIS_RIGHTY:
        if(controllers_.find(event.cdevice.which) != controllers_.end())
        {
          auto iter = controllers.find(event.cdevice.which);
          if(iter == controllers.end())
          {
            iter = controllers.emplace(event.cdevice.which, ControllerTemp{0, 0, 0, 0, 0, 0, 0, 0}).first;
          }
          iter->second.y_look_ += event.caxis.value;
          ++(iter->second.n_look_);
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
  else if(key_action_left)
  {
    action_left_();
  }
  else if(key_action_right)
  {
    action_right_();
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
      if((controller.second.m_move_) || (controller.second.n_move_))
      {
        if(controller.second.m_move_)
        {
          state->second.x_move_raw_ = static_cast<float>(controller.second.x_move_) / controller.second.m_move_;
        }
        if(controller.second.n_move_)
        {
          state->second.y_move_raw_ = static_cast<float>(controller.second.y_move_) / controller.second.n_move_;
        }

        float angle = std::atan2(state->second.y_move_raw_, state->second.x_move_raw_);
        float length = std::sqrt(state->second.x_move_raw_ * state->second.x_move_raw_ + state->second.y_move_raw_ * state->second.y_move_raw_) / INT16_MAX;
        length += update_offset_;
        length *= update_scale_;
        length = std::min(std::max(length, 0.f), 1.f);

        state->second.x_move_report_ = length * std::cos(angle);
        state->second.y_move_report_ = -length * std::sin(angle);
      }

      if((controller.second.m_look_) || (controller.second.n_look_))
      {
        if(controller.second.m_look_)
        {
          state->second.x_look_raw_ = static_cast<float>(controller.second.x_look_) / controller.second.m_look_;
        }
        if(controller.second.n_look_)
        {
          state->second.y_look_raw_ = static_cast<float>(controller.second.y_look_) / controller.second.n_look_;
        }

        float angle = std::atan2(state->second.y_look_raw_, state->second.x_look_raw_);
        float length = std::sqrt(state->second.x_look_raw_ * state->second.x_look_raw_ + state->second.y_look_raw_ * state->second.y_look_raw_) / INT16_MAX;
        length += update_offset_;
        length *= update_scale_;
        length = std::min(std::max(length, 0.f), 1.f);

        state->second.x_look_report_ = length * std::cos(angle);
        state->second.y_look_report_ = -length * std::sin(angle);
      }
    }
  }

  float x_move_report = 0.f;
  float y_move_report = 0.f;
  bool move_update = false;

  if(!controllers_.empty())
  {
    int n = 0;
    for(auto& controller : controllers_)
    {
      if((controller.second.x_move_report_ != 0.f) || (controller.second.y_move_report_ != 0.f))
      {
        x_move_report += controller.second.x_move_report_;
        y_move_report += controller.second.y_move_report_;
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
      x_move_report *= scale;
      y_move_report *= scale;
      move_update = true;
    }
  }

  if(key_move)
  {
    float length;
    if((key_move_state_left_ != key_move_state_right_) && (key_move_state_up_ != key_move_state_down_))
    {
      length = std::sqrt(.5f);
    }
    else
    {
      length = 1.f;
    }

    if(key_move_state_left_ != key_move_state_right_)
    {
      if(key_move_state_right_)
      {
        x_move_key_ = length;
      }
      else
      {
        x_move_key_ = -length;
      }
    }
    else
    {
      x_move_key_ = 0.f;
    }

    if(key_move_state_up_ != key_move_state_down_)
    {
      if(key_move_state_up_)
      {
        y_move_key_ = length;
      }
      else
      {
        y_move_key_ = -length;
      }
    }
    else
    {
      y_move_key_ = 0.f;
    }

    x_move_report += x_move_key_;
    y_move_report += y_move_key_;
    move_update = true;
  }
  else if(key_move_state_up_ || key_move_state_down_ || key_move_state_left_ || key_move_state_right_)
  {
    x_move_report += x_move_key_;
    y_move_report += y_move_key_;
    move_update = true;
  }

  if(move_update)
  { 
    float dx = x_move_report_ - x_move_report;
    float dy = y_move_report_ - y_move_report;
    if((dx * dx + dy * dy) > update_threshold_)
    {
      x_move_report_ = x_move_report;
      y_move_report_ = y_move_report;
  
      for(auto iter = move_report_.begin(); iter != move_report_.end();)
      {
        if((*iter)(x_move_report_, y_move_report_))
        {
          ++iter;
        }
        else
        {
          iter = move_report_.erase(iter);
        }
      }
    }
  }

  float x_look_report = 0.f;
  float y_look_report = 0.f;
  bool look_update = false;

  if(!controllers_.empty())
  {
    int n = 0;
    for(auto& controller : controllers_)
    {
      if((controller.second.x_look_report_ != 0.f) || (controller.second.y_look_report_ != 0.f))
      {
        x_look_report += controller.second.x_look_report_;
        y_look_report += controller.second.y_look_report_;
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
      x_look_report *= scale;
      y_look_report *= scale;
      look_update = true;
    }
  }

  if(key_look)
  {
    float length;
    if((key_look_state_left_ != key_look_state_right_) && (key_look_state_up_ != key_look_state_down_))
    {
      length = std::sqrt(.5f);
    }
    else
    {
      length = 1.f;
    }

    if(key_look_state_left_ != key_look_state_right_)
    {
      if(key_look_state_right_)
      {
        x_look_key_ = length;
      }
      else
      {
        x_look_key_ = -length;
      }
    }
    else
    {
      x_look_key_ = 0.f;
    }

    if(key_look_state_up_ != key_look_state_down_)
    {
      if(key_look_state_up_)
      {
        y_look_key_ = length;
      }
      else
      {
        y_look_key_ = -length;
      }
    }
    else
    {
      y_look_key_ = 0.f;
    }

    x_look_report += x_look_key_;
    y_look_report += y_look_key_;
    look_update = true;
  }
  else if(key_look_state_up_ || key_look_state_down_ || key_look_state_left_ || key_look_state_right_)
  {
    x_look_report += x_look_key_;
    y_look_report += y_look_key_;
    look_update = true;
  }

  if(look_update)
  { 
    float dx = x_look_report_ - x_look_report;
    float dy = y_look_report_ - y_look_report;
    if((dx * dx + dy * dy) > update_threshold_)
    {
      x_look_report_ = x_look_report;
      y_look_report_ = y_look_report;
  
      for(auto iter = look_report_.begin(); iter != look_report_.end();)
      {
        if((*iter)(x_look_report_, y_look_report_))
        {
          ++iter;
        }
        else
        {
          iter = look_report_.erase(iter);
        }
      }
    }
  }
}

Event::Event(lua::Stack& lua) : impl_(std::make_shared<Impl>(lua))
{
}

auto Event::operator()() -> void
{
  impl_->Check();
}

auto Event::Control(Command const& command) -> void
{
  impl_->Control(command);
}

auto Event::Look(Command const& command) -> void
{
  impl_->Look(command);
}

auto Event::ChoiceUp(event::Command const& command) -> void
{
  impl_->ChoiceUp(command);
}

auto Event::ChoiceDown(event::Command const& command) -> void
{
  impl_->ChoiceDown(command);
}

auto Event::ChoiceLeft(event::Command const& command) -> void
{
  impl_->ChoiceLeft(command);
}

auto Event::ChoiceRight(event::Command const& command) -> void
{
  impl_->ChoiceRight(command);
}

auto Event::ActionLeft(event::Command const& command) -> void
{
  impl_->ActionLeft(command);
}

auto Event::ActionRight(event::Command const& command) -> void
{
  impl_->ActionRight(command);
}

auto Event::Select(event::Command const& command) -> void
{
  impl_->Select(command);
}

auto Event::Back(event::Command const& command) -> void
{
  impl_->Back(command);
}

auto Event::Quit(event::Command const& command) -> void
{
  impl_->Quit(command);
}

Event::operator bool() const
{
  return static_cast<bool>(impl_);
}
}