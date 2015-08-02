#include "collision_group.h"
#include "signal.h"
#include "bind.h"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
namespace
{
struct BodyRecord
{
  dynamics::Body::WeakPtr body_;
  std::string group_;

  BodyRecord(dynamics::Body::WeakPtr const& body, std::string const& group) : body_(body), group_(group)
  {
  }
};

typedef boost::multi_index_container
<
  BodyRecord,
  boost::multi_index::indexed_by
  <
    boost::multi_index::ordered_unique
    <
      boost::multi_index::composite_key
      <
        BodyRecord,
        boost::multi_index::member<BodyRecord, dynamics::Body::WeakPtr, &BodyRecord::body_>,
        boost::multi_index::member<BodyRecord, std::string, &BodyRecord::group_>
      >
    >
  >
> BodyMap;

struct GroupRecord
{
  std::string group_a_;
  std::string group_b_;
  event::Signal begin_;
  event::Signal end_;
  GroupRecord(std::string const& group_a, std::string const& group_b) : group_a_(group_a), group_b_(group_b) 
  {
  }
};

typedef boost::multi_index_container
<
  GroupRecord,
  boost::multi_index::indexed_by
  <
    boost::multi_index::hashed_unique
    <
      boost::multi_index::composite_key
      <
        GroupRecord,
        boost::multi_index::member<GroupRecord, std::string, &GroupRecord::group_a_>,
        boost::multi_index::member<GroupRecord, std::string, &GroupRecord::group_b_>
      >
    >,
    boost::multi_index::hashed_non_unique
    <
      boost::multi_index::member<GroupRecord, std::string, &GroupRecord::group_a_>
    >,
    boost::multi_index::hashed_non_unique
    <
      boost::multi_index::member<GroupRecord, std::string, &GroupRecord::group_b_>
    >
  >
> GroupMap;

auto Find(GroupMap const& collisions, std::string const& group_a, std::string const& group_b) -> GroupMap::nth_index<0>::type::iterator
{
  if(group_a < group_b)
  {
    return collisions.find(boost::make_tuple(group_a, group_b));
  }
  else
  {
    return collisions.find(boost::make_tuple(group_b, group_a));
  }
}
}

namespace collision
{
class Group::Impl
{
public:
  Impl(event::Queue& queue);
  auto Begin(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void;
  auto End(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void;
  auto Link(std::string const& group_a, dynamics::Body const& body_a) -> void;
  auto Unlink(std::string const& group_a, dynamics::Body const& body_a) -> void;
  auto Link(std::string const& group_a, std::string const& group_b) -> void;
  auto Unlink(std::string const& group_a, std::string const& group_b) -> void;
  auto Check(dynamics::Body const& body_a, dynamics::Body const& body_b) -> bool;
  auto Begin(dynamics::Body const& body_a, dynamics::Body const& body_b) -> void;
  auto End(dynamics::Body const& body_a, dynamics::Body const& body_b) -> void;
  auto Unlink(dynamics::Body const& body_a) -> void;
  auto Unlink(std::string const& group_a) -> void;
  BodyMap members_;
  GroupMap collisions_;
  event::Queue queue_;
};

Group::Impl::Impl(event::Queue& queue) : queue_(queue)
{
}

auto Group::Impl::Begin(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void
{
  auto iter = Find(collisions_, group_a, group_b);
  if(iter != collisions_.end())
  {
    collisions_.modify(iter, [&](GroupRecord& record){record.begin_.Add(command);});
  }
}

auto Group::Impl::End(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void
{
  auto iter = Find(collisions_, group_a, group_b);
  if(iter != collisions_.end())
  {
    collisions_.modify(iter, [&](GroupRecord& record){record.end_.Add(command);});
  }
}

auto Group::Impl::Link(std::string const& group_a, dynamics::Body const& body_a) -> void
{
  members_.emplace(body_a, group_a);
}

auto Group::Impl::Unlink(std::string const& group_a, dynamics::Body const& body_a) -> void
{
  auto iter = members_.find(boost::make_tuple(body_a, group_a));
  if(iter != members_.end())
  {
    members_.erase(iter);
  }
}

auto Group::Impl::Link(std::string const& group_a, std::string const& group_b) -> void
{
  if(group_a < group_b)
  {
    collisions_.emplace(group_a, group_b);
  }
  else
  {
    collisions_.emplace(group_b, group_a);
  }
}

auto Group::Impl::Unlink(std::string const& group_a, std::string const& group_b) -> void
{
  auto iter = Find(collisions_, group_a, group_b);
  if(iter != collisions_.end())
  {
    collisions_.erase(iter);
  }
}

auto Group::Impl::Check(dynamics::Body const& body_a, dynamics::Body const& body_b) -> bool
{
  auto range_a = members_.equal_range(body_a);
  auto range_b = members_.equal_range(body_b);
  for(auto j = range_b.first; j != range_b.second; ++j)
  {
    for(auto i = range_a.first; i != range_a.second; ++i)
    {
      auto iter = Find(collisions_, i->group_, j->group_);
      if(iter != collisions_.end())
      {
        return true;
      }
    }
  }
  return false;
}

auto Group::Impl::Begin(dynamics::Body const& body_a, dynamics::Body const& body_b) -> void
{
  auto range_a = members_.equal_range(body_a);
  auto range_b = members_.equal_range(body_b);
  for(auto j = range_b.first; j != range_b.second; ++j)
  {
    for(auto i = range_a.first; i != range_a.second; ++i)
    {
      auto iter = Find(collisions_, i->group_, j->group_);
      if(iter != collisions_.end())
      {
        collisions_.modify(iter, [&](GroupRecord& record){record.begin_(queue_);});
      }
    }
  }
}

auto Group::Impl::End(dynamics::Body const& body_a, dynamics::Body const& body_b) -> void
{
  auto range_a = members_.equal_range(body_a);
  auto range_b = members_.equal_range(body_b);
  for(auto j = range_b.first; j != range_b.second; ++j)
  {
    for(auto i = range_a.first; i != range_a.second; ++i)
    {
      auto iter = Find(collisions_, i->group_, j->group_);
      if(iter != collisions_.end())
      {
        collisions_.modify(iter, [&](GroupRecord& record){record.end_(queue_);});
      }
    }
  }
}

auto Group::Impl::Unlink(dynamics::Body const& body_a) -> void
{
  auto range_a = members_.equal_range(body_a);
  members_.erase(range_a.first, range_a.second);
}

auto Group::Impl::Unlink(std::string const& group_a) -> void
{
  collisions_.get<1>().erase(group_a);
  collisions_.get<2>().erase(group_a);
}

Group::Group(event::Queue& queue) : impl_(std::make_shared<Impl>(queue))
{
}

auto Group::Begin(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void
{
  impl_->Begin(group_a, group_b, command);
}

auto Group::End(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void
{
  impl_->End(group_a, group_b, command);
}

auto Group::Link(std::string const& group_a, dynamics::Body const& body_a) -> void
{
  impl_->Link(group_a, body_a);
}

auto Group::Unlink(std::string const& group_a, dynamics::Body const& body_a) -> void
{
  impl_->Unlink(group_a, body_a);
}

auto Group::Link(std::string const& group_a, std::string const& group_b) -> void
{
  impl_->Link(group_a, group_b);
}

auto Group::Unlink(std::string const& group_a, std::string const& group_b) -> void
{
  impl_->Unlink(group_a, group_b);
}

auto Group::Check(dynamics::Body const& body_a, dynamics::Body const& body_b) -> bool
{
  return impl_->Check(body_a, body_b);
}

auto Group::Begin(dynamics::Body const& body_a, dynamics::Body const& body_b) -> void
{
  impl_->Begin(body_a, body_b);
}

auto Group::End(dynamics::Body const& body_a, dynamics::Body const& body_b) -> void
{
  impl_->End(body_a, body_b);
}

auto Group::Unlink(dynamics::Body const& body_a) -> void
{
  impl_->Unlink(body_a);
}

auto Group::Unlink(std::string const& group_a) -> void
{
  impl_->Unlink(group_a);
}
}