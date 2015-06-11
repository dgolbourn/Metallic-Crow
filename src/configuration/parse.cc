#include "parse.h"
#include "boost/program_options.hpp"
#include <iostream>
#include <fstream>
#include "exception.h"
#include "version.h"
namespace config
{
OptionalArgs Parse(int argc, char* argv[])
{
  boost::program_options::options_description general("General options");
  general.add_options()
    ("version,v", "display version string")
    ("help,h", "display help message");

  boost::program_options::options_description config("Configuration");
  config.add_options()
    ("config,c", boost::program_options::value<std::string>()->composing(), "control configuration file")
    ("root,r", boost::program_options::value<std::string>()->composing(), "root filesystem path");
    
  boost::program_options::options_description hidden("Hidden options");
  hidden.add_options()
    ("initialisation", boost::program_options::value<std::string>(), "initialisation file");

  boost::program_options::options_description command_line;
  command_line.add(general).add(config).add(hidden);

  boost::program_options::options_description config_file;
  config_file.add(config);

  boost::program_options::options_description visible("Allowed options");
  visible.add(general).add(config);

  boost::program_options::positional_options_description positional;
  positional.add("initialisation", -1);

  boost::program_options::variables_map variables;
  boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(command_line).positional(positional).allow_unregistered().run(), variables);
  boost::program_options::notify(variables);

  if(variables.count("initialisation"))
  {
    std::string initialisation_file = variables["initialisation"].as<std::string>();
    std::ifstream file(initialisation_file);
    if(file)
    {
      boost::program_options::store(boost::program_options::parse_config_file(file, config_file, true), variables);
      boost::program_options::notify(variables);
    }
    else
    {
      BOOST_THROW_EXCEPTION(exception::Exception());
    }
  }

  OptionalArgs args;

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
  else if(variables.count("root"))
  {
    args = Args();
    args->path = variables["root"].as<std::string>();
    
    if(variables.count("config"))
    {
      args->config = variables["config"].as<std::string>();
      valid = true;
      help = false;
    }
  }

  if(!valid)
  {
    args.reset();
  }  
  if(help)
  {
    std::cout << visible << std::endl;
  }
  return args;
}
}