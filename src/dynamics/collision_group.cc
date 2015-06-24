#include "collision_group.h"
#include <map>
#include <set>
#include "signal.h"
#include "bind.h"
namespace
{
typedef std::map<std::string, std::set<dynamics::Body::WeakPtr>> Members;
typedef std::map<std::string, std::map<std::string, std::pair<event::Signal, event::Signal>>> Collisions;
}

namespace collision
{
class Group::Impl
{
public:
  Impl(lua::Stack& lua, Collision const& collision);
  auto Add(std::string const& group_a, std::string const& group_b, event::Command const& command, bool begin) -> void;
  auto Add(std::string const& group_a, dynamics::Body const& body_a) -> void;
  Members members_;
  Collisions collisions_;
  Collision collision_;
};

Group::Impl::Impl(lua::Stack& lua, Collision const& collision) : collision_(collision)
{
  for(int index = 1, end = lua.Size(); index <= end; ++index)
  {
    lua::Guard guard = lua.Field(index);

    std::string group_a;
    {
      lua::Guard guard = lua.Field(1);
      lua.Pop(group_a);
    }

    std::string group_b;
    {
      lua::Guard guard = lua.Field(2);
      lua.Pop(group_b);
    }

    collisions_[group_a][group_b] = collisions_[group_b][group_a];
    members_[group_a];
    members_[group_b];
  }
}

auto Group::Impl::Add(std::string const& group_a, std::string const& group_b, event::Command const& command, bool begin) -> void
{
  auto iter_a = collisions_.find(group_a);
  if(iter_a != collisions_.end())
  {
    auto iter_b = iter_a->second.find(group_b);
    if(iter_b != iter_a->second.end())
    {
      if(begin)
      {
        iter_b->second.first.Add(command);
      }
      else
      {
        iter_b->second.second.Add(command);
      }
    }
  }
}

auto Group::Impl::Add(std::string const& group_a, dynamics::Body const& body_a) -> void
{
  auto members_iter = members_.find(group_a);
  if(members_iter != members_.end())
  { 
    if(members_iter->second.insert(body_a).second)
    {
      auto body_a_iter = collisions_.find(group_a);
      if(body_a_iter != collisions_.end())
      { 
        for(auto group_b : body_a_iter->second)
        {
          auto body_b_set = members_.find(group_b.first);
          if(body_b_set != members_.end())
          {
            for(auto body_b_iter = body_b_set->second.begin(); body_b_iter != body_b_set->second.end();)
            {
              if(auto body_b = body_b_iter->Lock())
              {
                typedef void(event::Signal::*Notify)();
                collision_.Link(body_a, body_b);
                collision_.Begin(body_a, body_b, function::Bind((Notify)&event::Signal::operator(), group_b.second.first));
                collision_.End(body_a, body_b, function::Bind((Notify)&event::Signal::operator(), group_b.second.second));
                ++body_b_iter;
              }
              else
              {
                collision_.Unlink(body_b);
                body_b_iter = body_b_set->second.erase(body_b_iter);
              }
            }
          }
        }
      }
    }
  }
}

auto Group::Begin(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void
{
  impl_->Add(group_a, group_b, command, true);
}

auto Group::End(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void
{
  impl_->Add(group_a, group_b, command, false);
}

auto Group::Add(std::string const& group_a, dynamics::Body const& body_a) -> void
{
  impl_->Add(group_a, body_a);
}

Group::Group(lua::Stack& lua, Collision const& collision) : impl_(std::make_shared<Impl>(lua, collision))
{
}
}