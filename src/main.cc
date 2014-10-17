#include "parse.h"
#include "bind.h"
#include "log.h"
#include "event.h"
#include "controller.h"
namespace
{
bool run = true;
bool Quit(void)
{
  run = false;
  return false;
}
}

int main(int argc, char* argv[])
{
  int ret = EXIT_FAILURE;
  try
  {
    if(config::Args args = config::Parse(argc, argv))
    {
      event::Event event(json::JSON(args->first));
      event::Queue queue;
      game::Controller controller(json::JSON(args->second), queue);

      queue.Add(function::Bind(&event::Event::operator(), event));
      
      controller.Add(Quit);

      event.Quit(Quit);
      event.Select(function::Bind(&game::Controller::Select, controller), event::None);
      event.Up(function::Bind(&game::Controller::Up, controller), function::Bind(&game::Controller::Down, controller));
      event.Down(function::Bind(&game::Controller::Down, controller), function::Bind(&game::Controller::Up, controller));
      event.Left(function::Bind(&game::Controller::Left, controller), function::Bind(&game::Controller::Right, controller));
      event.Right(function::Bind(&game::Controller::Right, controller), function::Bind(&game::Controller::Left, controller));
      event.ChoiceUp(function::Bind(&game::Controller::ChoiceUp, controller), event::None);
      event.ChoiceDown(function::Bind(&game::Controller::ChoiceDown, controller), event::None);
      event.ChoiceLeft(function::Bind(&game::Controller::ChoiceLeft, controller), event::None);
      event.ChoiceRight(function::Bind(&game::Controller::ChoiceRight, controller), event::None);

      while(run)
      {
        queue();
      }
    }
    ret = EXIT_SUCCESS;
  }
  catch(...)
  {
    exception::Log("Unhandled exception");
  }
  return ret;
}
