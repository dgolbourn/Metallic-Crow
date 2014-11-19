#include "collision_group.h"
#include <map>
#include <set>
#include "switch.h"
#include "json_iterator.h"
#include "bind.h"
namespace collision
{
namespace
{
typedef std::map<std::string, std::set<dynamics::Body::WeakPtr>> Members;
typedef std::map<std::string, std::map<std::string, event::Switch>> Collisions;
}

class Group::Impl
{
public:
  Impl(json::JSON const& json, Collision const& collision);
  void Add(std::string const& group_a, std::string const& group_b, event::Command const& command, bool begin);
  void Add(std::string const& group_a, dynamics::Body const& body_a);
  Members members_;
  Collisions collisions_;
  Collision collision_;
};

Group::Impl::Impl(json::JSON const& json, Collision const& collision) : collision_(collision)
{
  json_t* collisions;

  json.Unpack("{so}", 
    "allowed collisions", 
    &collisions);

  for(auto& element : json::JSON(collisions))
  {
    char const* group_a;
    char const* group_b;
    element.Unpack("[ss]", &group_a, &group_b);
    collisions_[group_a][group_b] = collisions_[group_b][group_a];
  }
}

void Group::Impl::Add(std::string const& group_a, std::string const& group_b, event::Command const& command, bool begin)
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

void Group::Impl::Add(std::string const& group_a, dynamics::Body const& body_a)
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

void Group::Begin(std::string const& group_a, std::string const& group_b, event::Command const& command)
{
  impl_->Add(group_a, group_b, command, true);
}

void Group::End(std::string const& group_a, std::string const& group_b, event::Command const& command)
{
  impl_->Add(group_a, group_b, command, false);
}

void Group::Add(std::string const& group_a, dynamics::Body const& body_a)
{
  impl_->Add(group_a, body_a);
}

Group::Group(json::JSON const& json, Collision const& collision) : impl_(std::make_shared<Impl>(json, collision))
{
}
}