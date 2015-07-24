#ifndef GAME_JOINT_H_
#define GAME_JOINT_H_
#include <memory>
#include "actor.h"
#include "lua_stack.h"
namespace game
{
class Joint
{
public:
  Joint() = default;
  static size_t Hash(Joint const& joint);
  bool operator==(Joint const& other) const;
  explicit operator bool() const; 
  Joint(lua::Stack& lua, Actor const& actor_a, Actor const& actor_b, dynamics::World& world);
  typedef memory::WeakPtr<Joint, class JointImpl> WeakPtr;
  friend WeakPtr;
private:
  std::shared_ptr<class JointImpl> impl_;
};
}

namespace std 
{
template<> struct hash<game::Joint> 
{
  size_t operator()(game::Joint const& joint) const 
  { 
    return game::Joint::Hash(joint); 
  }
};
}
#endif