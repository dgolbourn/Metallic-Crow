#ifndef COMMAND_H_
#define COMMAND_H_
#include <memory>
namespace event
{
class CommandImpl
{
public:
  virtual void operator()(void) = 0;
};
typedef std::shared_ptr<CommandImpl> Command;
}
#endif