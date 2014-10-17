#include "parse.h"
#include "boost/program_options.hpp"
#include <iostream>
#include <fstream>
#include "exception.h"
#include "version.h"
namespace config
{
Args Parse(int argc, char* argv[])
{
  namespace options = boost::program_options;

  options::options_description general("General options");
  general.add_options()
    ("version,v", "display version string")
    ("help,h", "display help message");

  options::options_description config("Configuration");
  config.add_options()
    ("control,c", options::value<std::string>()->composing(), "control configuration file")
    ("game,g", options::value<std::string>()->composing(), "game configuration file");

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
  options::store(options::command_line_parser(argc, argv).options(command_line).positional(positional).allow_unregistered().run(), variables);
  options::notify(variables);

  if(variables.count("initialisation"))
  {
    std::string initialisation_file = variables["initialisation"].as<std::string>();
    std::ifstream file(initialisation_file);
    if(file)
    {
      options::store(options::parse_config_file(file, config_file, true), variables);
      options::notify(variables);
    }
    else
    {
      BOOST_THROW_EXCEPTION(exception::Exception());
    }
  }

  std::string control_file;
  std::string game_file;
  
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
  else if(variables.count("control"))
  {
    control_file = variables["control"].as<std::string>();
    if(variables.count("game"))
    {
      game_file = variables["game"].as<std::string>();
      valid = true;
      help = false;
    }
  }

  Args args;
  if(valid)
  {
    args = std::make_pair(control_file, game_file);
  }
  else if(help)
  {
    std::cout << visible << std::endl;
  }
  return args;
}
}