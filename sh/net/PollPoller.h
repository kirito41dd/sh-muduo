#ifndef SH_NET_POLLPOLLER_H
#define SH_NET_POLLPOLLER_H

#include "sh/net/Poller.h"

#include <vector>

struct pollfd;

namespace sh
{

namespace net
{

/// IO多路转接 poll(2)
class PollPoller : public Poller
{
public:
    PollPoller(EventLoop *loop);
    ~PollPoller() override = default;

    TimeStamp poll(int timeoutMs, ChannelList *activeChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;
private:
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    typedef std::vector<struct pollfd> PollFdList;
    PollFdList pollfds_;
}; // class PollPoller

} // namespace net

} // namespace sh

#endif // SH_NET_POLLPOLLER_H
