#ifndef COLLISION_GROUP_H_
#define COLLISION_GROUP_H_
#include <list>
#include <map>
#include <set>
#include "collision.h"
namespace game
{
template<class Channel, bool start> class CollisionGroup
{
public:
  typedef std::pair<dynamics::Body::WeakPtr, Channel> Member;
  typedef std::list<Member> MemberList;
  typedef std::map<int, MemberList> MemberMap;
  typedef std::set<int> LinkSet;
  typedef std::map<int, LinkSet> LinkMap;
  Collision collision_;
  MemberMap members_;
  LinkMap links_;

  void Link(int group_a, int group_b)
  {
    if(group_a > group_b)
    {
      std::swap(group_a, group_b);
    }
    if(links_[group_a].insert(group_b).second)
    {
      (void)links_[group_b].insert(group_a);
      MemberList& members_a = members_[group_a];
      MemberList& members_b = members_[group_b];
      for(auto iter_a = members_a.begin(); iter_a != members_a.end();)
      {
        if(dynamics::Body body_a = iter_a->first.Lock())
        {
          for(auto iter_b = members_b.begin(); iter_b != members_b.end();)
          {
            if(dynamics::Body body_b = iter_b->first.Lock())
            {
              collision_.Add(body_a, body_b, event::Bind(iter_a->second, iter_b->second), start);
              ++iter_b;
            }
            else
            {
              iter_b = members_b.erase(iter_b);
            }
          }
          ++iter_a;
        }
        else
        {
          iter_a = members_a.erase(iter_a);
        }
      }
    }
  }

  void Add(int this_group, dynamics::Body const& this_body, Channel const& channel)
  {
    for(int group : links_[this_group])
    {
      MemberList& members = members_[group];
      for(auto iter = members.begin(); iter != members.end();)
      {
        if(dynamics::Body body = iter->first.Lock())
        {
          collision_.Add(this_body, body, event::Bind(iter->second, channel), start);
          ++iter;
        }
        else
        {
          iter = members.erase(iter);
        }
      }
    }
    members_[this_group].push_back(Member(this_body, channel));
  }

  CollisionGroup(Collision const& collision) : collision_(collision)
  {
  }
};
}
#endif