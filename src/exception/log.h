#ifndef LOG_H_
#define LOG_H_
#include <string>
namespace exception
{
auto Log(std::string const& detail) noexcept -> void;
}
#endif