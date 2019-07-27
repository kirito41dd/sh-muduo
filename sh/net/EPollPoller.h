#ifndef SH_NET_EPOLLPOLLER_H
#define SH_NET_EPOLLPOLLER_H

#include "sh/net/Poller.h"

#include <vector>

struct epoll_event;

namespace sh
{

namespace net
{

/// IO多路转接 epoll(4)
class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop *loop);
    ~EPollPoller() override;

    TimeStamp poll(int timeoutMs, ChannelList *activeChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;
private:
    static const int kInitEventListSize = 16;
    static const char *operationToString(int op);

    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;

    void update(int operation, Channel *channel);

    typedef std::vector<struct epoll_event> EventList;
    int         epollfd_;
    EventList   events_;
}; // class EPollPoller

} // namespace net

} // namespace sh

#endif // SH_NET_EPOLLPOLLER_H
