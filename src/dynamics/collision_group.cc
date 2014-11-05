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
  void Begin(std::string const& group_a, std::string const& group_b, event::Command const& command);
  void End(std::string const& group_a, std::string const& group_b, event::Command const& command);
  void Link(std::string const& group_a, std::string const& group_b);
  void Add(std::string const& group, dynamics::Body const& body);
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

void Group::Impl::Begin(std::string const& group_a, std::string const& group_b, event::Command const& command)
{
  Link(group_a, group_b);
  collisions_[group_a][group_b].first.Add(command);
}

void Group::Impl::End(std::string const& group_a, std::string const& group_b, event::Command const& command)
{
  Link(group_a, group_b);
  collisions_[group_a][group_b].second.Add(command);
}

void Group::Impl::Link(std::string const& group_a, std::string const& group_b)
{
  bool link = true;

  auto iter_a = collisions_.find(group_a);
  if(iter_a != collisions_.end())
  {
    auto iter_b = collisions_.find(group_b);
    if(iter_b != collisions_.end())
    {
      link = false;
    }
  }

  if(link)
  {
    collisions_[group_a][group_b] = collisions_[group_b][group_a];

    auto& member_a_set = members_[group_a];
    for(auto iter_aa = member_a_set.begin(); iter_aa != member_a_set.end();)
    {
      if(auto body_a = iter_aa->Lock())
      {
        auto& member_b_set = members_[group_b];
        for(auto iter_bb = member_b_set.begin(); iter_bb != member_b_set.end();)
        {
          if(auto body_b = iter_bb->Lock())
          {
            auto signals = collisions_[group_b][group_a];
            typedef void (event::Signal::*Notify)();
            collision_.Begin(body_a, body_b, function::Bind((Notify)&event::Signal::operator(), signals.first));
            collision_.End(body_a, body_b, function::Bind((Notify)&event::Signal::operator(), signals.second));
            ++iter_bb;
          }
          else
          {
            iter_bb = member_a_set.erase(iter_bb);
            collision_.Unlink(*iter_bb);
          }
        }
        ++iter_aa;
      }
      else
      {
        iter_aa = member_a_set.erase(iter_aa);
        collision_.Unlink(*iter_aa);
      }
    }
  }
}

void Group::Impl::Add(std::string const& group, dynamics::Body const& body)
{
  if(members_[group].insert(body).second)
  {
    for(auto iter : collisions_[group])
    {
      auto& body_set = members_[iter.first];
      for(auto other_iter = body_set.begin(); other_iter != body_set.end();)
      {
        if(auto other = other_iter->Lock())
        {
          typedef void (event::Signal::*Notify)();
          collision_.Begin(body, other, function::Bind((Notify)&event::Signal::operator(), iter.second.first));
          collision_.End(body, other, function::Bind((Notify)&event::Signal::operator(), iter.second.first));
          ++other_iter;
        }
        else
        {
          other_iter = body_set.erase(other_iter);
          collision_.Unlink(*other_iter);
        }
      }
    }
  }
}

void Group::Begin(std::string const& group_a, std::string const& group_b, event::Command const& command)
{
  impl_->Begin(group_a, group_b, command);
}

void Group::End(std::string const& group_a, std::string const& group_b, event::Command const& command)
{
  impl_->End(group_a, group_b, command);
}

void Group::Link(std::string const& group_a, std::string const& group_b)
{
  impl_->Link(group_a, group_b);
}

void Group::Add(std::string const& group, dynamics::Body const& body)
{
  impl_->Add(group, body);
}

Group::Group(json::JSON const& json, Collision const& collision) : impl_(std::make_shared<Impl>(json, collision))
{
}
}