#include "log.h"
#include <iostream>
#include "boost/exception/diagnostic_information.hpp"
namespace exception
{
void Log(std::string const& detail)
{
  std::cerr << detail << std::endl << boost::current_exception_diagnostic_information();
}
}