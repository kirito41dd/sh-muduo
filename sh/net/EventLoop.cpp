
#include "sh/net/EventLoop.h"

#include "sh/base/Logging.h"
#include "sh/net/Channel.h"
#include "sh/base/Mutex.h"
#include "sh/net/Poller.h"
#include "sh/net/SocketOps.h"

#include <sys/eventfd.h>

namespace
{

__thread sh::net::EventLoop *t_loopInThisThread = 0;

const int kPollTimeMs = 10000;

int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_SYSERR << "Failed in eventfd";
        abort();
    }
    return evtfd;
}

} // namespace

namespace sh
{

namespace net
{

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      eventHandling_(false),
      callingPendingFunctors_(false),
      iteration_(0),
      threadId_(CurrentThread::tid()),
      poller_(Poller::newDefaultPoller(this)),
      //**timerQueue_(NULL/* add later */),
      wakeupFd_(createEventfd()),
      wakeupChannel_(new Channel(this, wakeupFd_)),
      currentActiveChannel_(NULL)
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

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = sockets::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void EventLoop::updateChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    //**poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{

}

bool EventLoop::hasChannel(Channel *channel)
{

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
