#ifndef MAKE_CHANNEL_H_
#define MAKE_CHANNEL_H_
#include "channel.h"
namespace dynamics
{
class Message
{
};
typedef event::Send<Message> Send;
typedef event::Receive<Message> Receive;
typedef event::Channel<Message> Channel;

Channel MakeChannel(event::Command const& command);
Channel MakeChannel(void);
}
#endif