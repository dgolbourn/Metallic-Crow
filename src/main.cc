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
    if(config::OptionalArgs args = config::Parse(argc, argv))
    {
      event::Event event(json::JSON(args->control));
      event::Queue queue;
      game::Controller controller(json::JSON(args->game), queue, args->path);

      queue.Add(function::Bind(&event::Event::operator(), event));
      
      controller.Add(Quit);

      event.Quit(Quit);
      event.Select(function::Bind(&game::Controller::Select, controller));
      event.Back(function::Bind(&game::Controller::Back, controller));
      event.Control(function::Bind(&game::Controller::Control, controller));
      event.ChoiceUp(function::Bind(&game::Controller::ChoiceUp, controller));
      event.ChoiceDown(function::Bind(&game::Controller::ChoiceDown, controller));
      event.ChoiceLeft(function::Bind(&game::Controller::ChoiceLeft, controller));
      event.ChoiceRight(function::Bind(&game::Controller::ChoiceRight, controller));

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
