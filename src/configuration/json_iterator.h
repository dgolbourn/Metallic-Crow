#ifndef JSON_ITERATOR_H_
#define JSON_ITERATOR_H_
#include "json.h"
#include "boost/iterator/iterator_facade.hpp"
namespace json
{
JSON::Iterator begin(JSON& json);
JSON::Iterator end(JSON const& json);

class JSON::Iterator final : public boost::iterator_facade<Iterator, JSON const, boost::forward_traversal_tag>
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