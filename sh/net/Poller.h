#ifndef SH_NET_POLLER_H
#define SH_NET_POLLER_H

#include <map>
#include <vector>

#include "sh/base/TimeStamp.h"
#include "sh/net/EventLoop.h"

namespace sh
{

namespace net
{

class Channel;

/// IO Multiplexing 的基类
/// 这个类不拥有Channel
class Poller : noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;

    Poller(EventLoop *loop);
    virtual ~Poller() = default;

    /// polls the IO events
    /// 必须在loop的线程中调用
    virtual TimeStamp poll(int timeoutMs, ChannelList *activeChannels) = 0;

    /// 改变关心的IO events
    /// 必须在loop的线程中调用
    virtual void updateChannel(Channel *channel) = 0;

    /// Remove the channel, when it destructs.
    /// 必须在loop的线程中调用
    virtual void removeChannel(Channel *channel) = 0;

    virtual bool hasChannel(Channel *channel) const;

    static Poller* newDefaultPoller(EventLoop *loop); // in DefaultPoller.cpp

    void assertInLoopThread() const
    {
        ownerLoop_->assertInLoopThread();
    }

protected:
    typedef std::map<int, Channel*> ChannelMap;
    ChannelMap channels_;
private:
    EventLoop *ownerLoop_;
}; // class Poller

} // namespace net

} // namespace sh

#endif // SH_NET_POLLER_H
