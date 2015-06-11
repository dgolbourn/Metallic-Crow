#include<memory>
namespace lua
{
class Stack;
class StackImpl;

class Guard
{
private:
  StackImpl* impl_;
  friend StackImpl;
  friend Stack;
  explicit Guard(StackImpl* impl);
public:
  Guard(Guard&& other);
  Guard& operator=(Guard&& other);
  ~Guard();
};
}