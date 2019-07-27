#ifndef SH_NET_EVENTLOOP_H
#define SH_NET_EVENTLOOP_H

#include "sh/base/Mutex.h"
#include "sh/base/CurrentThread.h"
#include "sh/base/TimeStamp.h"


namespace sh
{

namespace net
{

class EventLoop : noncopyable
{
public:

    EventLoop();
    ~EventLoop();

    /// 必须在创建对象的线程调用，进入循环
    void loop();


    // pid_t threadId() const { return threadId_; }
    void assertInLoopThread()
    {
        if (!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }


    static EventLoop* getEventLoopOfCurrentThread();

private:

    void abortNotInLoopThread();

    bool            looping_; /* atomic */
    const pid_t     threadId_;
}; // class EventLoop

} // namespace net

} // namespace sh

#endif // SH_NET_EVENTLOOP_H
