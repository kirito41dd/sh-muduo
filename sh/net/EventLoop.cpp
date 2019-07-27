
#include "sh/net/EventLoop.h"

#include "sh/base/Logging.h"

namespace
{

__thread sh::net::EventLoop *t_loopInThisThread = 0;

} // namespace

namespace sh
{

namespace net
{

EventLoop::EventLoop()
    : looping_(false),
      threadId_(CurrentThread::tid())
{

}

EventLoop::~EventLoop()
{

}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;

    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_ = false;
}

EventLoop *EventLoop::getEventLoopOfCurrentThread()
{
    return ::t_loopInThisThread;
}

void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in threadId_ = " << threadId_
              << ", current thread id = " << CurrentThread::tid();
}

} // namespace net

} // namespace sh
