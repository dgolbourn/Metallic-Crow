#include "now.h"
#include <sstream>
#include <ctime>
#include <iomanip>
#include <mutex>
namespace calendar
{
std::string Now()
{
  static std::mutex mutex;
  std::lock_guard<std::mutex> lock(mutex);
  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);
  std::stringstream stream;
  stream << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
  return stream.str();
}
}