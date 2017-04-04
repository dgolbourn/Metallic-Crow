#include "event.h"
#include "SDL.h"
#include "SDL_events.h"
#include "SDL_scancode.h"
#include "signal.h"
#include "sdl_library.h"
#include <unordered_map>
#include <cmath>
#include "sdl_exception.h"
#include "log.h"
#include "lua_exception.h"
#include <algorithm>
#include "for_each.h"
namespace
{
typedef std::list<event::Event::Control> Controls;
typedef std::list<event::Event::Button> Buttons;
typedef std::list<event::Event::Switch> Switches;
typedef std::list<event::Event::Index> Indices;

struct Coords
{
  float x_;
  float y_;

  void Reset()
  {
    x_ = 0.f;
    y_ = 0.f;
  }
};

struct Axis
{
  Sint32 sum_;
  int n_;

  void Add(Sint16 increment)
  {
    sum_ += increment;
    ++n_;
  }

  float Mean()
  {
    return static_cast<float>(sum_) / n_;
  }

  explicit operator bool()
  {
    return n_ != 0;
  }

  void Reset()
  {
    sum_ = 0;
    n_ = 0;
  }
};

struct Axes
{
  Axis x_;
  Axis y_;

  explicit operator bool()
  {
    return x_ || y_;
  }

  void Reset()
  {
    x_.Reset();
    y_.Reset();
  }
};

struct SDLController
{
  SDL_GameController* controller_;

  SDLController() : controller_(nullptr)
  {
  }

  void Reset(SDL_GameController* controller)
  {
    controller_ = controller;
  }

  ~SDLController()
  {
    if(controller_)
    {
      SDL_GameControllerClose(controller_);
    }
  }
};

struct ControllerAxes
{
  Axes axes_;
  Coords coords_;

  void Reset()
  {
    axes_.Reset();
    coords_.Reset();
  }

  void Update(Controls& controls, int id, float offset, float scale, float threshold)
  {
    if(axes_)
    {
      float x = axes_.x_.Mean();
      float y = axes_.y_.Mean();
      float angle = std::atan2(y, x);
      float length = std::sqrt(x * x + y * y) / INT16_MAX;
      length += offset;
      length *= scale;
      length = std::min(std::max(length, 0.f), 1.f);
      x = length * std::cos(angle);
      y = -length * std::sin(angle);
      float dx = coords_.x_ - x;
      float dy = coords_.y_ - y;
      if((dx * dx + dy * dy) > threshold)
      {
        coords_.x_ = x;
        coords_.y_ = y;
        event::for_each(controls, id, x, y);
      }
      axes_.Reset();
    }
  }
};

struct Switch
{
  bool next_;
  bool current_;
  bool updated_;

  void Reset()
  {
    next_ = false;
    current_ = false;
    updated_ = false;
  }

  explicit operator bool()
  {
    return updated_;
  }

  void On()
  {
    next_ = true;
    if(!updated_ && !current_)
    {
      updated_ = true;
    }
  }

  void Off()
  {
    next_ = false;
    if(!updated_ && current_)
    {
      updated_ = true;
    }
  }

  void Update(Switches& switches, int id)
  {
    if(updated_)
    {
      event::for_each(switches, id, next_);
      if(next_ != current_)
      {
        event::for_each(switches, id, current_);
        current_ = next_;
      }
      updated_ = false;
    }
  }
};

struct Button
{
  bool state_;

  explicit operator bool()
  {
    return state_;
  }

  void Reset()
  {
    state_ = false;
  }

  void On()
  {
    state_ = true;
  }

  void Update(Buttons& buttons, int id)
  {
    if(state_)
    {
      event::for_each(buttons, id);
      state_ = false;
    }
  }
};

struct Report
{
  Buttons choice_up_;
  Buttons choice_down_;
  Buttons choice_left_;
  Buttons choice_right_;
  Buttons back_;
  Buttons select_;
  Switches action_left_;
  Switches action_right_;
  Controls move_;
  Controls look_;
  Indices add_;
  Indices remove_;
};

struct ControllerState
{
  SDLController controller_;
  ControllerAxes move_;
  ControllerAxes look_;
  Button choice_up_;
  Button choice_down_;
  Button choice_left_;
  Button choice_right_;
  Button back_;
  Button select_;
  Switch action_left_;
  Switch action_right_;

  void Reset(SDL_GameController* controller)
  {
    controller_.Reset(controller);
    move_.Reset();
    look_.Reset();
    choice_up_.Reset();
    choice_down_.Reset();
    choice_left_.Reset();
    choice_right_.Reset();
    back_.Reset();
    select_.Reset();
    action_left_.Reset();
    action_right_.Reset();
  }

  void Update(Report& report, int id, float offset, float scale, float threshold)
  {
    move_.Update(report.move_, id, offset, scale, threshold);
    look_.Update(report.look_, id, offset, scale, threshold);
    choice_up_.Update(report.choice_up_, id);
    choice_down_.Update(report.choice_down_, id);
    choice_left_.Update(report.choice_left_, id);
    choice_right_.Update(report.choice_right_, id);
    back_.Update(report.back_, id);
    select_.Update(report.select_, id);
    action_left_.Update(report.action_left_, id);
    action_right_.Update(report.action_right_, id);
  }
};

typedef std::unordered_map<SDL_JoystickID, ControllerState> ControllerStateMap;

struct KeyState
{
  bool next_;
  bool current_;

  void Reset()
  {
    next_ = false;
    current_ = false;
  }

  void Update()
  {
    current_ = next_;
  }

  explicit operator bool()
  {
    return current_ != next_;
  }

  void On()
  {
    next_ = true;
  }

  void Off()
  {
    next_ = false;
  }

  bool State()
  {
    return next_;
  }
};

struct KeyAxis
{
  KeyState inc_;
  KeyState dec_;

  explicit operator bool()
  {
    return static_cast<bool>(inc_) != static_cast<bool>(dec_);
  }

  void Update()
  {
    inc_.Update();
    dec_.Update();
  }

  void Reset()
  {
    inc_.Reset();
    dec_.Reset();
  }

  int State()
  {
    return static_cast<int>(inc_.State()) - static_cast<int>(dec_.State());
  }
};

struct KeyAxes
{
  KeyAxis x_;
  KeyAxis y_;

  explicit operator bool()
  {
    return x_ || y_;
  }

  void Reset()
  {
    x_.Reset();
    y_.Reset();
  }

  void Update(Controls& controls, int id)
  {
    if(x_ || y_)
    {
      float x_length = static_cast<float>(x_.State());
      float y_length = static_cast<float>(y_.State());

      if(x_length && y_length)
      {
        static const float scale = std::sqrt(.5f);
        x_length *= scale;
        y_length *= scale;
      }

      event::for_each(controls, id, x_length, y_length);
    }
    x_.Update();
    y_.Update();
  }
};

struct KeyController
{
  KeyAxes move_;
  KeyAxes look_;
  Button choice_up_;
  Button choice_down_;
  Button choice_left_;
  Button choice_right_;
  Button back_;
  Button select_;
  Switch action_left_;
  Switch action_right_;

  void Reset()
  {
    move_.Reset();
    look_.Reset();
    choice_up_.Reset();
    choice_down_.Reset();
    choice_left_.Reset();
    choice_right_.Reset();
    back_.Reset();
    select_.Reset();
    action_left_.Reset();
    action_right_.Reset();
  }

  void Update(Report& report, int id)
  {
    move_.Update(report.move_, id);
    look_.Update(report.look_, id);
    choice_up_.Update(report.choice_up_, id);
    choice_down_.Update(report.choice_down_, id);
    choice_left_.Update(report.choice_left_, id);
    choice_right_.Update(report.choice_right_, id);
    back_.Update(report.back_, id);
    select_.Update(report.select_, id);
    action_left_.Update(report.action_left_, id);
    action_right_.Update(report.action_right_, id);
  }
};

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
  auto Check() ->void;
  auto Move(Control const& control) -> void;
  auto Look(Control const& control) -> void;
  auto ChoiceUp(Button const& button) -> void;
  auto ChoiceDown(Button const& button) -> void;
  auto ChoiceLeft(Button const& button) -> void;
  auto ChoiceRight(Button const& button) -> void;
  auto ActionLeft(Switch const& zwitch) -> void;
  auto ActionRight(Switch const& zwitch) -> void;
  auto Select(Button const& button) -> void;
  auto Back(Button const& button) -> void;
  auto Quit(Command const& command) -> void;
  auto Add(Index const& id) -> void;
  auto Remove(Index const& id) -> void;
  auto Controllers() -> std::vector<int>;
  sdl::Library sdl_;
  Report report_;
  Signal quit_;
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
  SDL_Scancode key_back_;
  SDL_Scancode key_select_;
  SDL_Scancode key_action_left_;
  SDL_Scancode key_action_right_;
  ControllerStateMap controllers_;
  KeyController controller_;
  float offset_;
  float scale_;
  float threshold_;
};

auto Event::Impl::Move(Control const& control) -> void
{
  report_.move_.push_back(control);
}

auto Event::Impl::Look(Control const& control) -> void
{
  report_.look_.push_back(control);
}

auto Event::Impl::ChoiceUp(Button const& button) -> void
{
  report_.choice_up_.push_back(button);
}

auto Event::Impl::ChoiceDown(Button const& button) -> void
{
  report_.choice_down_.push_back(button);
}

auto Event::Impl::ChoiceLeft(Button const& button) -> void
{
  report_.choice_left_.push_back(button);
}

auto Event::Impl::ChoiceRight(Button const& button) -> void
{
  report_.choice_right_.push_back(button);
}

auto Event::Impl::ActionLeft(Switch const& zwitch) -> void
{
  report_.action_left_.push_back(zwitch);
}

auto Event::Impl::ActionRight(Switch const& zwitch) -> void
{
  report_.action_right_.push_back(zwitch);
}

auto Event::Impl::Select(Button const& button) -> void
{
  report_.select_.push_back(button);
}

auto Event::Impl::Back(Button const& button) -> void
{
  report_.back_.push_back(button);
}

auto Event::Impl::Quit(Command const& command) -> void
{
  quit_.Add(command);
}

auto Event::Impl::Add(Index const& id) -> void
{
  report_.add_.push_back(id);
}

auto Event::Impl::Remove(Index const& id) -> void
{
  report_.remove_.push_back(id);
}

Event::Impl::Impl(lua::Stack& lua) : sdl_(SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER)
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

  controller_.Reset();
  for(int i = 0; i < SDL_NumJoysticks(); ++i)
  {
    if(SDL_IsGameController(i))
    {
      if(SDL_GameController* game_controller = SDL_GameControllerOpen(i))
      {
        SDL_JoystickID joystick = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(game_controller));
        if(joystick == -1)
        {
          BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
        }
        controllers_[joystick].Reset(game_controller);
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

  offset_ = -static_cast<float>(min);
  scale_ = static_cast<float>(1. / (max - min));
  threshold_ = static_cast<float>(threshold * threshold);
}

auto Event::Impl::Check() -> void
{
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
          controller_.move_.y_.inc_.On();
        }
        else if(code == key_move_down_)
        {
          controller_.move_.y_.dec_.On();
        }
        else if(code == key_move_left_)
        {
          controller_.move_.x_.dec_.On();
        }
        else if(code == key_move_right_)
        {
          controller_.move_.x_.inc_.On();
        }
        else if(code == key_look_up_)
        {
          controller_.look_.y_.inc_.On();
        }
        else if(code == key_look_down_)
        {
          controller_.look_.y_.dec_.On();
        }
        else if(code == key_look_left_)
        {
          controller_.look_.x_.dec_.On();
        }
        else if(code == key_look_right_)
        {
          controller_.look_.x_.inc_.On();
        }
        else if(code == key_choice_up_)
        {
          controller_.choice_up_.On();
        }
        else if(code == key_choice_down_)
        {
          controller_.choice_down_.On();
        }
        else if(code == key_choice_left_)
        {
          controller_.choice_left_.On();
        }
        else if(code == key_choice_right_)
        {
          controller_.choice_right_.On();
        }       
        else if(code == key_action_left_)
        {
          controller_.action_left_.On();
        }
        else if(code == key_action_right_)
        {
          controller_.action_right_.On();
        }
        else if(code == key_select_)
        {
          controller_.select_.On();
        }
        else if(code == key_back_)
        {
          controller_.back_.On();
        }
      }
      break;
    case SDL_KEYUP:
      if(event.key.repeat == 0)
      {
        SDL_Scancode code = event.key.keysym.scancode;
        if(code == key_move_up_)
        {
          controller_.move_.y_.inc_.Off();
        }
        else if(code == key_move_down_)
        {
          controller_.move_.y_.dec_.Off();
        }
        else if(code == key_move_left_)
        {
          controller_.move_.x_.dec_.Off();
        }
        else if(code == key_move_right_)
        {
          controller_.move_.x_.inc_.Off();
        }
        else if(code == key_look_up_)
        {
          controller_.look_.y_.inc_.Off();
        }
        else if(code == key_look_down_)
        {
          controller_.look_.y_.dec_.Off();
        }
        else if(code == key_look_left_)
        {
          controller_.look_.x_.dec_.Off();
        }
        else if(code == key_look_right_)
        {
          controller_.look_.x_.inc_.Off();
        }       
        else if(code == key_action_left_)
        {
          controller_.action_left_.Off();
        }
        else if(code == key_action_right_)
        {
          controller_.action_right_.Off();
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
        controllers_[joystick].Reset(game_controller);
        for_each(report_.add_, joystick);
      }
      else
      {
        BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
      }
      break;
    case SDL_CONTROLLERDEVICEREMOVED:
      controllers_.erase(event.cdevice.which);
      for_each(report_.remove_, event.cdevice.which);
      break;
    case SDL_CONTROLLERBUTTONDOWN:
      switch(event.cbutton.button)
      {
      case SDL_CONTROLLER_BUTTON_A:
        controllers_[event.cbutton.which].choice_down_.On();
        break;
      case SDL_CONTROLLER_BUTTON_B:
        controllers_[event.cbutton.which].choice_right_.On();
        break;
      case SDL_CONTROLLER_BUTTON_X:
        controllers_[event.cbutton.which].choice_left_.On();
        break;
      case SDL_CONTROLLER_BUTTON_Y:
        controllers_[event.cbutton.which].choice_up_.On();
        break;
      case SDL_CONTROLLER_BUTTON_BACK:
        controllers_[event.cbutton.which].back_.On();
        break;
      case SDL_CONTROLLER_BUTTON_START:
        controllers_[event.cbutton.which].select_.On();
        break;
      case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        controllers_[event.cbutton.which].action_left_.On();
        break;
      case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        controllers_[event.cbutton.which].action_right_.On();
        break;
      default:
        break;
      }
      break;
    case SDL_CONTROLLERBUTTONUP:
      switch(event.cbutton.button)
      {
      case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        controllers_[event.cbutton.which].action_left_.Off();
        break;
      case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        controllers_[event.cbutton.which].action_right_.Off();
        break;
      default:
        break;
      }
      break;
    case SDL_CONTROLLERAXISMOTION:
      switch(event.caxis.axis)
      {
      case SDL_CONTROLLER_AXIS_LEFTX:
        controllers_[event.caxis.which].move_.axes_.x_.Add(event.caxis.value);
        break;
      case SDL_CONTROLLER_AXIS_LEFTY:
        controllers_[event.caxis.which].move_.axes_.y_.Add(event.caxis.value);
        break;
      case SDL_CONTROLLER_AXIS_RIGHTX:
        controllers_[event.caxis.which].look_.axes_.x_.Add(event.caxis.value);
        break;
      case SDL_CONTROLLER_AXIS_RIGHTY:
        controllers_[event.caxis.which].look_.axes_.y_.Add(event.caxis.value);
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
  }

  controller_.Update(report_, -1);
  for(auto& controller : controllers_)
  {
    controller.second.Update(report_, controller.first, offset_, scale_, threshold_);
  }
}

auto Event::Impl::Controllers() -> std::vector<int>
{
  std::vector<int> controllers;
  controllers.push_back(-1);
  for(auto& controller : controllers_)
  {
    controllers.push_back(controller.first);
  }
  std::sort(controllers.begin(), controllers.end());
  return controllers;
}

Event::Event(lua::Stack& lua) : impl_(std::make_shared<Impl>(lua))
{
}

auto Event::operator()() -> void
{
  impl_->Check();
}

auto Event::Move(Control const& control) -> void
{
  impl_->Move(control);
}

auto Event::Look(Control const& control) -> void
{
  impl_->Look(control);
}

auto Event::ChoiceUp(Button const& button) -> void
{
  impl_->ChoiceUp(button);
}

auto Event::ChoiceDown(Button const& button) -> void
{
  impl_->ChoiceDown(button);
}

auto Event::ChoiceLeft(Button const& button) -> void
{
  impl_->ChoiceLeft(button);
}

auto Event::ChoiceRight(Button const& button) -> void
{
  impl_->ChoiceRight(button);
}

auto Event::ActionLeft(Switch const& zwitch) -> void
{
  impl_->ActionLeft(zwitch);
}

auto Event::ActionRight(Switch const& zwitch) -> void
{
  impl_->ActionRight(zwitch);
}

auto Event::Select(Button const& button) -> void
{
  impl_->Select(button);
}

auto Event::Back(Button const& button) -> void
{
  impl_->Back(button);
}

auto Event::Quit(Command const& command) -> void
{
  impl_->Quit(command);
}

auto Event::Controllers() -> std::vector<int>
{
  return impl_->Controllers();
}

auto Event::Add(Index const& id) -> void
{
  impl_->Add(id);
}

auto Event::Remove(Index const& id) -> void
{
  impl_->Remove(id);
}

Event::operator bool() const
{
  return static_cast<bool>(impl_);
}
}