#ifndef INPUT_H_
#define INPUT_H_
#include <memory>
namespace event
{
class Input
{
public:
  Input() = default;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif