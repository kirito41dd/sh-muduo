
#include "sh/net/Poller.h"

#include "sh/net/Channel.h"

using namespace sh;
using namespace sh::net;

Poller::Poller(EventLoop *loop)
    : ownerLoop_(loop)
{
}


bool Poller::hasChannel(Channel *channel) const
{
    assertInLoopThread();
    ChannelMap::const_iterator it = channels_.find(channel->fd());
    return it != channels_.end() && it->second == channel;
}



