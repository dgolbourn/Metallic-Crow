#ifndef CONFIG_H_
#define CONFIG_H_
#include <string>
#include "boost/optional.hpp"
namespace config
{
typedef boost::optional<std::pair<std::string, std::string>> Args;
Args Parse(int argc, char* argv[]);
}
#endif