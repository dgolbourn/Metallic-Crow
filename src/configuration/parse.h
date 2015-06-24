#ifndef PARSE_H_
#define PARSE_H_
#include <string>
#include "boost/optional.hpp"
#include "boost/filesystem.hpp"
namespace config
{
struct Args
{
  boost::filesystem::path config;
  boost::filesystem::path path;
};
typedef boost::optional<Args> OptionalArgs;

auto Parse(int argc, char* argv[]) -> OptionalArgs;
}
#endif