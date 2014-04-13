#include "make_channel.h"
namespace dynamics
{
static const Message message;

static const Send send = [=](void)
{
  return std::pair<Message, bool>(message, true);
};

static const Receive receive = [=](Message)
{
  return true;
};

static const Channel channel(send, receive);

Channel MakeChannel(void)
{
  return channel;
}

Channel MakeChannel(event::Command const& command)
{
  Send send = [=](void)
  {
    return std::pair<Message, bool>(message, command());
  };
  return Channel(send, receive);
}
}