#include "event.h"
#include "SDL.h"
#include "SDL_events.h"
#include "SDL_scancode.h"
#include <unordered_map>
#include "signal.h"
#include "sdl_library.h"
#include "switch.h"
namespace event
{
typedef std::unordered_map<SDL_Scancode, Switch> KeyMap;

class EventImpl
{
public:
  EventImpl(json::JSON const& json);
  void Check(void);
  void Up(Command const& start, Command const& end);
  void Down(Command const& start, Command const& end);
  void Left(Command const& start, Command const& end);
  void Right(Command const& start, Command const& end);
  void ChoiceUp(Command const& start, Command const& end);
  void ChoiceDown(Command const& start, Command const& end);
  void ChoiceLeft(Command const& start, Command const& end);
  void ChoiceRight(Command const& start, Command const& end);
  void Pause(Command const& command);
  void Resume(Command const& command);
  void Quit(Command const& command);
  void Pause(void);
  void Resume(void);
  void Keydown(SDL_KeyboardEvent const& keyboard_event, Uint32 type);
  sdl::Library const sdl_;
  Switch up_;
  Switch down_;
  Switch left_;
  Switch right_;
  Switch choice_up_;
  Switch choice_down_;
  Switch choice_left_;
  Switch choice_right_;
  Switch quit_;
  Switch pause_;
  Switch resume_;
  KeyMap keys_;
};

void EventImpl::Up(Command const& start, Command const& end)
{
  up_.first.Add(start);
  up_.second.Add(end);
}

void EventImpl::Down(Command const& start, Command const& end)
{
  down_.first.Add(start);
  down_.second.Add(end);
}

void EventImpl::Left(Command const& start, Command const& end)
{
  left_.first.Add(start);
  left_.second.Add(end);
}

void EventImpl::Right(Command const& start, Command const& end)
{
  right_.first.Add(start);
  right_.second.Add(end);
}

void EventImpl::ChoiceUp(Command const& start, Command const& end)
{
  choice_up_.first.Add(start);
  choice_up_.second.Add(end);
}

void EventImpl::ChoiceDown(Command const& start, Command const& end)
{
  choice_down_.first.Add(start);
  choice_down_.second.Add(end);
}

void EventImpl::ChoiceLeft(Command const& start, Command const& end)
{
  choice_left_.first.Add(start);
  choice_left_.second.Add(end);
}

void EventImpl::ChoiceRight(Command const& start, Command const& end)
{
  choice_right_.first.Add(start);
  choice_right_.second.Add(end);
}

void EventImpl::Pause(Command const& command)
{
  pause_.first.Add(command);
}

void EventImpl::Resume(Command const& command)
{
  resume_.first.Add(command);
}

void EventImpl::Quit(Command const& command)
{
  quit_.first.Add(command);
}

void EventImpl::Pause(void)
{
  pause_.first();
}

void EventImpl::Resume(void)
{
  resume_.first();
}

void EventImpl::Keydown(SDL_KeyboardEvent const& keyboard_event, Uint32 type)
{
  if(keyboard_event.repeat == 0)
  {
    auto iter = keys_.find(keyboard_event.keysym.scancode);
    if(iter != keys_.end())
    {
      switch(type)
      {
      case SDL_KEYDOWN:
        iter->second.first();
        break;
      case SDL_KEYUP:
        iter->second.second();
        break;
      }
    }
  }
}

EventImpl::EventImpl(json::JSON const& json) : sdl_(SDL_INIT_EVENTS)
{
  int up, down, left, right, choice_up, choice_down, choice_left, choice_right, pause, resume, quit;
  json.Unpack("{sisisisisisisisisisisi}",
    "up", &up,
    "down", &down,
    "left", &left,
    "right", &right,
    "choice up", &choice_up,
    "choice down", &choice_down,
    "choice left", &choice_left,
    "choice right", &choice_right,
    "pause", &pause,
    "resume", &resume,
    "quit", &quit);

  keys_[SDL_Scancode(up)] = up_;
  keys_[SDL_Scancode(down)] = down_;
  keys_[SDL_Scancode(left)] = left_;
  keys_[SDL_Scancode(right)] = right_;
  keys_[SDL_Scancode(choice_up)] = choice_up_;
  keys_[SDL_Scancode(choice_down)] = choice_down_;
  keys_[SDL_Scancode(choice_left)] = choice_left_;
  keys_[SDL_Scancode(choice_right)] = choice_right_;
  keys_[SDL_Scancode(pause)] = pause_;
  keys_[SDL_Scancode(resume)] = resume_;
  keys_[SDL_Scancode(quit)] = quit_;
}

void EventImpl::Check(void)
{
  SDL_Event event;
  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
    case SDL_QUIT:
      quit_.first();
      break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      Keydown(event.key , event.type);
      break;
    default:
      break;
    }
  }
}

Event::Event(json::JSON const& json)
{
  impl_ = std::make_shared<EventImpl>(json);
}

void Event::operator()(void)
{
  impl_->Check();
}

void Event::Up(Command const& start, Command const& end)
{
  impl_->Up(start, end);
}

void Event::Down(Command const& start, Command const& end)
{
  impl_->Down(start, end);
}

void Event::Left(Command const& start, Command const& end)
{
  impl_->Left(start, end);
}

void Event::Right(Command const& start, Command const& end)
{
  impl_->Right(start, end);
}

void Event::ChoiceUp(Command const& start, Command const& end)
{
  impl_->ChoiceUp(start, end);
}

void Event::ChoiceDown(Command const& start, Command const& end)
{
  impl_->ChoiceDown(start, end);
}

void Event::ChoiceLeft(Command const& start, Command const& end)
{
  impl_->ChoiceLeft(start, end);
}

void Event::ChoiceRight(Command const& start, Command const& end)
{
  impl_->ChoiceRight(start, end);
}

void Event::Pause(Command const& command)
{
  impl_->Pause(command);
}

void Event::Resume(Command const& command)
{
  impl_->Resume(command);
}

void Event::Quit(Command const& command)
{
  impl_->Quit(command);
}

void Event::Pause(void)
{
  impl_->Pause();
}

void Event::Resume(void)
{
  impl_->Resume();
}

Event::operator bool(void) const
{
  return bool(impl_);
}
}