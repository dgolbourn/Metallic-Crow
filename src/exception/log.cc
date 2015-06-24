#include "log.h"
#include <iostream>
#include "boost/exception/diagnostic_information.hpp"
namespace exception
{
auto Log(std::string const& detail) noexcept -> void
{
  try
  {
    std::cerr << detail << std::endl << boost::current_exception_diagnostic_information();
  }
  catch(...)
  {
  }
}
}