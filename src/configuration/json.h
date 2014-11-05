#ifndef JSON_H_
#define JSON_H_
#include <string>
#include "boost/filesystem.hpp"
struct json_t;
namespace json
{
class JSON
{
public:
  JSON();
  explicit JSON(json_t* json);
  explicit JSON(boost::filesystem::path const& file);
  template<class... Args> void Unpack(std::string const& format, Args... args) const
  {
    Unpack_(format, 0, args...);
  }

  ~JSON();
  JSON(JSON const& other);
  JSON(JSON&& other);
  JSON& operator=(JSON other);
  explicit operator bool() const;
  bool operator==(JSON const& other) const;
  JSON operator[](int index);
  class Iterator;
  int Size() const;
  void Write(boost::filesystem::path const& file) const;
private:
  void Unpack_(std::string const& format, int dummy, ...) const;
  struct pack_tag_ {};
  JSON(pack_tag_, std::string const& format, int dummy, ...);
  json_t* json_;
public:
  template<class... Args> JSON(std::string const& format, Args... args) : JSON(pack_tag_(), format, 0, args...)
  {
  }
};
}
#endif