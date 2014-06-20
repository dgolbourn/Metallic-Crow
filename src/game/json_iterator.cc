#include "json_iterator.h"
namespace json
{ 
JSON::Iterator begin(JSON& json) 
{
  return JSON::Iterator(json);
}

JSON::Iterator end(JSON const&)
{
  return JSON::Iterator();
}

JSON::Iterator::Iterator(void) : index_(0)
{
}

JSON::Iterator::Iterator(JSON& json) : json_(json), value_(json_[0]), index_(0)
{
}

void JSON::Iterator::increment(void)
{
  value_ = json_[++index_];
}

bool JSON::Iterator::equal(Iterator const& other) const
{
  return (!json_ && !other.json_) || (!value_ && !other.value_) || ((json_ == other.json_) && (value_ == other.value_));
}

JSON const& JSON::Iterator::dereference(void) const
{
  return value_;
}
}