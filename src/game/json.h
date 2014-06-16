#ifndef JSON_H_
#define JSON_H_

#include <string>
#include <boost/iterator/iterator_facade.hpp>
struct json_t;

namespace json
{
class JSON
{
public:
  JSON(void);
  explicit JSON(json_t* json);
  explicit JSON(std::string const& filename);
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
  Iterator begin(void);
  Iterator end(void) const;
  int Size(void) const;
private:
  void Unpack_(std::string const& format, int dummy, ...) const;
  json_t* json_;
};

class JSON::Iterator : public boost::iterator_facade<Iterator, JSON const, boost::forward_traversal_tag>
{
public:
  Iterator(void);
  explicit Iterator(JSON& json);
private:
  friend class boost::iterator_core_access;
  void increment(void);
  bool equal(Iterator const& other) const;
  JSON const& dereference(void) const;
  JSON json_;
  JSON value_;
  int index_;
};
}
#endif