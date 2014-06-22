#include "parse.h"
#include "boost/program_options.hpp"
#include <iostream>
#include <fstream>
#include "exception.h"
#include "version.h"
namespace config
{
bool Parse(int argc, char* argv[], event::Event& event, display::Window& window, game::Script& script, event::Queue& queue)
{
  namespace options = boost::program_options;

  options::options_description general("General options");
  general.add_options()
    ("version,v", "display version string")
    ("help,h", "display help message");

  options::options_description config("Configuration");
  config.add_options()
    ("event,e", options::value<std::string>()->composing(), "event configuration file")
    ("window,w", options::value<std::string>()->composing(), "window configuration file")
    ("script,s", options::value<std::string>()->composing(), "script configuration file");

  options::options_description hidden("Hidden options");
  hidden.add_options()
    ("initialisation", options::value<std::string>(), "initialisation file");

  options::options_description command_line;
  command_line.add(general).add(config).add(hidden);

  options::options_description config_file;
  config_file.add(config);

  options::options_description visible("Allowed options");
  visible.add(general).add(config);

  options::positional_options_description positional;
  positional.add("initialisation", -1);

  options::variables_map variables;
  options::store(options::command_line_parser(argc, argv).options(command_line).positional(positional).run(), variables);
  options::notify(variables);

  if(variables.count("initialisation"))
  {
    std::string initialisation_file = variables["initialisation"].as<std::string>();
    std::ifstream file(initialisation_file);
    if(file)
    {
      options::store(options::parse_config_file(file, config_file), variables);
      options::notify(variables);
    }
    else
    {
      BOOST_THROW_EXCEPTION(exception::Exception());
    }
  }

  std::string event_file;
  std::string window_file;
  std::string script_file;

  bool valid = false;
  bool help = true;
  if(variables.count("help")) 
  {
  }
  else if(variables.count("version"))
  {
    std::cout << Version() << std::endl;
    valid = false;
    help = false;
  }
  else if(variables.count("event"))
  {
    event_file = variables["event"].as<std::string>();
    if(variables.count("window"))
    {
      window_file = variables["window"].as<std::string>();

      if(variables.count("script"))
      {
        script_file = variables["script"].as<std::string>();
        valid = true;
        help = false;
      }
    }
  }

  if(valid)
  {
    event = event::Event(json::JSON(event_file));
    window = display::Window(json::JSON(window_file));
    script = game::Script(json::JSON(script_file), window, queue);
  }
  else if(help)
  {
    std::cout << visible << std::endl;
  }
  return valid;
}
}