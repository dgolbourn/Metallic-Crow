#ifndef PARSE_H_
#define PARSE_H_
#include <string>
#include "boost/optional.hpp"
#include "boost/filesystem.hpp"
namespace config
{
struct Args
{
  boost::filesystem::path control;
  boost::filesystem::path path;
  boost::filesystem::path game;
};
typedef boost::optional<Args> OptionalArgs;

OptionalArgs Parse(int argc, char* argv[]);
}
#endif