#include "log.h"
#include <iostream>
#include "boost/exception/diagnostic_information.hpp"
#include "exception.h"
namespace exception
{
auto Log(std::string const& detail) noexcept -> void
{
  try
  {
    std::cerr << detail << "\n" << boost::current_exception_diagnostic_information();
    try
    {
      throw;
    }
    catch(Exception& exception)
    {
      std::exception_ptr *ptr = boost::get_error_info<Exception::Cause>(exception);
      if(ptr)
      {
        try
        {
          std::rethrow_exception(*ptr);
        }
        catch(...)
        {
          Log("CAUSE");
        }
      }
    }
  }
  catch(...)
  {
  }
}
}