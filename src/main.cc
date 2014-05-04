#include "event.h"
#include "scene.h"
#include "queue.h"
#include "collision.h"
#include "dynamics_collision.h"
#include "command_collision.h"
#include "world.h"
#include "subtitle.h"
#include "script.h"
#include "window.h"
#include "sync.h"
#include "boost/exception/diagnostic_information.hpp"

static bool run = true;
static bool Quit(void)
{
  run = false;
  return false;
}

int main(int argc, char* argv[])
{
  int ret;
  try
  {
    event::Event event(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/event.json"));
    event.Quit(Quit);
    display::Window window(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/window.json"));
    game::Scene scene(json::JSON("C:/Users/golbo_000/Documents/Visual Studio 2012/Projects/ReBassInvaders/resource/scene.json"), window);
    event::Queue queue;
    game::Collision collision(queue);
    game::DynamicsCollision dcollision(collision);
    game::CommandCollision ccollision(collision);
    dynamics::World world(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/world.json"), collision);
    game::Subtitle subtitle(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/subtitle.json"), window, scene, queue, event);
    game::Script script(json::JSON("C:/Users/golbo_000/Documents/GitHub/Metallic-Crow/res/script.json"), subtitle, window, scene, queue, dcollision, ccollision, world, event);

    subtitle.Resume();
    script.Resume();

    event::Sync sync(60.f);
    while(run)
    {
      window.Clear();
      scene.Render();
      window.Show();
      event();
      world.Step();
      queue();
      sync();
    }
    ret = EXIT_SUCCESS;
  }
  catch(...)
  {
    std::cerr << "Unhandled exception:" << std::endl << boost::current_exception_diagnostic_information();
    ret = EXIT_FAILURE;
  }
  return ret;
}
