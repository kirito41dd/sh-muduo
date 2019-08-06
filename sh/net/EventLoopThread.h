#ifndef SH_NET_EVENTLOOPTHREAD_H
#define SH_NET_EVENTLOOPTHREAD_H

#include "sh/base/Condition.h"
#include "sh/base/Mutex.h"
#include "sh/base/Thread.h"

namespace sh
{

namespace net
{

class EventLoop;

class EventLoopThread : noncopyable
{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThread(const ThreadInitCallback &cb = ThreadInitCallback(),
                    const string &name = string());
    ~EventLoopThread();
    EventLoop* startLoop();

private:
    void threadFunc();

    EventLoop           *loop_; // guarded by mutex_
    bool                exiting_;
    Thread              thread_;
    MutexLock           mutex_;
    Condition           cond_; // guarded by mutex_
    ThreadInitCallback  callback_;
}; // class EventLoopThread

} // namespace net

} // namespace sh

#endif // SH_NET_EVENTLOOPTHREAD_H
