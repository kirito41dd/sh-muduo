#ifndef SH_NET_EVENTLOOP_H
#define SH_NET_EVENTLOOP_H

#include "sh/base/Mutex.h"
#include "sh/base/CurrentThread.h"
#include "sh/base/TimeStamp.h"

#include <atomic>
#include <functional>
#include <vector>
#include <memory>

#include <boost/any.hpp>


namespace sh
{

namespace net
{

class Channel;
class Poller;
class TimerQueue;

class EventLoop : noncopyable
{
public:
    typedef std::function<void()> Functor;

    EventLoop();
    ~EventLoop();

    /// 必须在创建对象的线程调用，进入循环
    void loop();

    /// 退出循环
    /// 如果使用原生指针,不是100%线程安全,使用shared_ptr 100% safe
    void quit();

    /// Time when poll returns, 通常代表数据到来
    TimeStamp pollReturnTime() const { return pollReturnTime_; }

    int64_t iteration() const { return iteration_; }

    /// Runs callback immediately in the loop thread.
    /// It wakes up the loop, and run the cb.
    /// If in the same loop thread, cb is run within the function.
    /// Safe to call from other threads.
    void runInLoop(Functor cb);
    /// Queues callback in the loop thread.
    /// Runs after finish pooling.
    /// Safe to call from other threads.
    void queueInLoop(Functor cb);

    size_t queueSize() const;

    // timers

    // 内部使用
    void wakeup();
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    // pid_t threadId() const { return threadId_; }
    void assertInLoopThread()
    {
        if (!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
    // bool callingPendingFunctors() const { return callingPendingFunctors_; }
    bool eventHandling() const { return eventHandling_; }

    void setContext(const boost::any &context) { context_ = context; }
    const boost::any& getContext() const { return context_; }
    boost::any* getMutableContext() { return &context_; }

    static EventLoop* getEventLoopOfCurrentThread();

private:
    void abortNotInLoopThread();
    void handleRead(); // waked up
    void doPendingFunctors();

    void printActiveChannels() const; // debug

    typedef std::vector<Channel*> ChannelList;

    bool                        looping_; /* atomic */
    std::atomic<bool>           quit_;
    bool                        eventHandling_; /* atomic */
    bool                        callingPendingFunctors_; /* atomic */
    int64_t                     iteration_;
    const pid_t                 threadId_;
    TimeStamp                   pollReturnTime_;
    std::unique_ptr<Poller>     poller_;
    //**std::unique_ptr<TimerQueue> timerQueue_;
    int                         wakeupFd_;
    // unlike in TimerQueue, which is an internal class,
    // we don't expose Channel to client.
    std::unique_ptr<Channel>    wakeupChannel_;
    boost::any                  context_;

    // scratch variables
    ChannelList activeChannels_;
    Channel     *currentActiveChannel_;

    mutable MutexLock mutex_;
    std::vector<Functor> pendingFunctors_; // guarded by mutex_;

}; // class EventLoop

} // namespace net

} // namespace sh

#endif // SH_NET_EVENTLOOP_H
