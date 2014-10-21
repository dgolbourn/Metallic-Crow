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
      event.Back(function::Bind(&game::Controller::Back, controller), event::None);
      event.Up(function::Bind(&game::Controller::UpBegin, controller), function::Bind(&game::Controller::DownEnd, controller));
      event.Down(function::Bind(&game::Controller::DownBegin, controller), function::Bind(&game::Controller::UpEnd, controller));
      event.Left(function::Bind(&game::Controller::LeftBegin, controller), function::Bind(&game::Controller::RightEnd, controller));
      event.Right(function::Bind(&game::Controller::RightBegin, controller), function::Bind(&game::Controller::LeftEnd, controller));
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
