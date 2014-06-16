#include "json.h"
#include "va_list.h"
#include "jansson.h"
#include "json_exception.h"
namespace json
{
JSON::JSON(void)
{
  json_ = json_null();
}

JSON::JSON(json_t* json)
{
  json_ = json_incref(json);
}

JSON::JSON(std::string const& filename)
{
  json_error_t error;
  json_ = json_load_file(filename.c_str(), 0, &error);
  if(!json_)
  {
    BOOST_THROW_EXCEPTION(Exception() 
      << Exception::Text(error.text)
      << Exception::Source(error.source)
      << Exception::Line(error.line)
      << Exception::Column(error.column)
      << Exception::Position(error.position));
  }
}

JSON::~JSON(void)
{
  json_decref(json_);
}

JSON::JSON(JSON const& other)
{
  json_ = json_incref(other.json_);
}

JSON::JSON(JSON&& other) : JSON()
{
  std::swap(json_, other.json_);
}

JSON& JSON::operator=(JSON other)
{
  std::swap(json_, other.json_);
  return *this;
}

JSON::operator bool(void) const
{
  return !json_is_null(json_);
}

bool JSON::operator==(JSON const& other) const
{
  return json_equal(json_, other.json_) != 0;
}

void JSON::Unpack_(std::string const& format, int dummy, ...) const
{
  json_error_t error;
  if(json_vunpack_ex(json_, &error, JSON_STRICT, format.c_str(), cstd::VAList<int>(dummy)) == -1)
  {
    BOOST_THROW_EXCEPTION(Exception()
      << Exception::Text(error.text)
      << Exception::Source(error.source)
      << Exception::Line(error.line)
      << Exception::Column(error.column)
      << Exception::Position(error.position));
  }
}

int JSON::Size(void) const
{
  int ret = 0;
  if(json_is_array(json_))
  {
    ret = (int)json_array_size(json_);
  }
  return ret;
}

JSON JSON::operator[](int index)
{
  JSON ret;
  if(json_is_array(json_))
  {
    if((index >= 0) && (index < (int)json_array_size(json_)))
    {
      ret = JSON(json_array_get(json_, (size_t)index));
    }
  }
  return ret;
}

JSON::Iterator JSON::begin(void) 
{
  return JSON::Iterator(JSON(json_));
}

JSON::Iterator JSON::end(void) const
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