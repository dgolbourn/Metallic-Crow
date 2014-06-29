#include "parse.h"
#include "bind.h"
#include "boost/exception/diagnostic_information.hpp"
static bool run = true;
static bool Quit(void)
{
  run = false;
  return false;
}

static bool Render(display::Window::WeakPtr window_ptr, game::Script::WeakPtr script_ptr)
{
  bool valid = false;
  if(auto window = window_ptr.Lock())
  {
    if(auto script = script_ptr.Lock())
    {
      window.Clear();
      script.Render();
      window.Show();
      valid = true;
    }
  }
  return valid;
}

static bool Pause(game::Script::WeakPtr script_ptr)
{
  bool valid = false;
  if(auto script = script_ptr.Lock())
  {
    script.Pause();
    valid = true;
  }
  return valid;
}

static bool Resume(game::Script::WeakPtr script_ptr)
{
  bool valid = false;
  if(auto script = script_ptr.Lock())
  {
    script.Resume();
    valid = true;
  }
  return valid;
}

static bool Up(game::Script::WeakPtr script_ptr)
{
  bool valid = false;
  if(auto script = script_ptr.Lock())
  {
    script.Up();
    valid = true;
  }
  return valid;
}

static bool Down(game::Script::WeakPtr script_ptr)
{
  bool valid = false;
  if(auto script = script_ptr.Lock())
  {
    script.Down();
    valid = true;
  }
  return valid;
}

static bool Left(game::Script::WeakPtr script_ptr)
{
  bool valid = false;
  if(auto script = script_ptr.Lock())
  {
    script.Left();
    valid = true;
  }
  return valid;
}

static bool Right(game::Script::WeakPtr script_ptr)
{
  bool valid = false;
  if(auto script = script_ptr.Lock())
  {
    script.Right();
    valid = true;
  }
  return valid;
}

static bool ChoiceUp(game::Script::WeakPtr script_ptr)
{
  bool valid = false;
  if(auto script = script_ptr.Lock())
  {
    script.ChoiceUp();
    valid = true;
  }
  return valid;
}

static bool ChoiceDown(game::Script::WeakPtr script_ptr)
{
  bool valid = false;
  if(auto script = script_ptr.Lock())
  {
    script.ChoiceDown();
    valid = true;
  }
  return valid;
}

static bool ChoiceLeft(game::Script::WeakPtr script_ptr)
{
  bool valid = false;
  if(auto script = script_ptr.Lock())
  {
    script.ChoiceLeft();
    valid = true;
  }
  return valid;
}

static bool ChoiceRight(game::Script::WeakPtr script_ptr)
{
  bool valid = false;
  if(auto script = script_ptr.Lock())
  {
    script.ChoiceRight();
    valid = true;
  }
  return valid;
}

int main(int argc, char* argv[])
{
  int ret = EXIT_FAILURE;
  try
  {
    event::Event event;
    display::Window window;
    game::Script script;
    event::Queue queue;
    
    if(config::Parse(argc, argv, event, window, script, queue))
    {
      queue.Add(event::Bind(&event::Event::operator(), event));
      queue.Add(std::bind(Render, window, script));

      event.Quit(Quit);
      event.Pause(std::bind(Pause, script));
      event.Resume(std::bind(Resume, script));
      event.Up(std::bind(Up, script), std::bind(Down, script));
      event.Down(std::bind(Down, script), std::bind(Up, script));
      event.Left(std::bind(Left, script), std::bind(Right, script));
      event.Right(std::bind(Right, script), std::bind(Left, script));
      event.ChoiceUp(std::bind(ChoiceUp, script), event::None);
      event.ChoiceDown(std::bind(ChoiceDown, script), event::None);
      event.ChoiceLeft(std::bind(ChoiceLeft, script), event::None);
      event.ChoiceRight(std::bind(ChoiceRight, script), event::None);

      script.Resume();
      while(run)
      {
        queue();
      }
    }
    ret = EXIT_SUCCESS;
  }
  catch(...)
  {
    std::cerr << "Unhandled exception:" << std::endl << boost::current_exception_diagnostic_information();
  }
  return ret;
}
