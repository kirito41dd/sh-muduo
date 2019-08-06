#ifndef SH_NET_EVENTLOOPTHREADPOOL_H
#define SH_NET_EVENTLOOPTHREADPOOL_H

#include "sh/base/noncopyable.h"
#include "sh/base/Types.h"

#include <functional>
#include <memory>
#include <vector>

namespace sh
{

namespace net
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : noncopyable
{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThreadPool(EventLoop *baseLoop, const string &name);
    ~EventLoopThreadPool();
    void setThreadNum(int numThreads) { numThreads_ = numThreads; }
    void start(const ThreadInitCallback &cb = ThreadInitCallback());

    // 调用start后有效
    /// round-robin 轮询调度
    EventLoop* getNextLoop();

    /// with the same hash code, it will always return the same EventLoop
    /// hash % size 获取一个loop
    EventLoop* getLoopForHash(size_t hashCode);

    std::vector<EventLoop*> getAllLoops();

    bool started() const { return started_; }
    const string& name() const { return name_; }

private:

    EventLoop   *baseLoop_;
    string      name_;
    bool        started_;
    int         numThreads_;
    int         next_;
    std::vector<std::unique_ptr<EventLoopThread>>   threads_;
    std::vector<EventLoop*>                         loops_;
}; // class EventLoopThread

} // namespace net

} // namespace sh

#endif // SH_NET_EVENTLOOPTHREADPOOL_H
