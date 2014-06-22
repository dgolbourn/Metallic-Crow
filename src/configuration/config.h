#ifndef CONFIG_H_
#define CONFIG_H_
#include <string>
#include "event.h"
#include "world.h"
#include "queue.h"
#include "script.h"
namespace config
{
bool Parse(int argc, char* argv[], event::Event& event, display::Window& window, game::Script& script, event::Queue& queue);
}
#endif