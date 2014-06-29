#ifndef JSON_H_
#define JSON_H_
#include <string>
struct json_t;
namespace json
{
class JSON
{
public:
  JSON(void);
  explicit JSON(json_t* json);
  explicit JSON(std::string const& file);
  template<class... Args> void Unpack(std::string const& format, Args... args) const
  {
    Unpack_(format, 0, args...);
  }

  ~JSON(void);
  JSON(JSON const& other);
  JSON(JSON&& other);
  JSON& operator=(JSON other);
  explicit operator bool(void) const;
  bool operator==(JSON const& other) const;
  JSON operator[](int index);
  class Iterator;
  int Size(void) const;
private:
  void Unpack_(std::string const& format, int dummy, ...) const;
  json_t* json_;
};
}
#endif