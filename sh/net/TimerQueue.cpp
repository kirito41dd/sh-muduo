
// __STDC_LIMIT_MACROS 和 __STDC_CONSTANT_MACROS 是一种解决方案
// 允许 C++ 程序使用标准中指定的stdint.h 宏，而不在 C++ 中。
// 宏( 如 UINT8_MAX 。INT64_MIN 和 INT32_C() ) 可能已经在 C++ 程序中定定义其他方式。
// 为了允许用户決定是否需要为c 定义宏，
// 许多实现需要在 stdint.h 包含之前定义 __STDC_LIMIT_MACROS 和 __STDC_CONSTANT_MACROS 。
// 这不是 C++ 标准的一部分，但它已经被许多实现所采用。

// __STDC_LIMIT_MACROS and __STDC_CONSTANT_MACROS are a workaround to allow C++ programs to use stdint.h
// macros specified in the C99 standard that aren't in the C++ standard. The macros, such as UINT8_MAX, INT64_MIN,
// and INT32_C() may be defined already in C++ applications in other ways. To allow the user to decide
// if they want the macros defined as C99 does, many implementations require that __STDC_LIMIT_MACROS
// and __STDC_CONSTANT_MACROS be defined before stdint.h is included.
// This isn't part of the C++ standard, but it has been adopted by more than one implementation.

#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif

#include "sh/net/TimerQueue.h"

#include "sh/base/Logging.h"
#include "sh/net/EventLoop.h"
#include "sh/net/Timer.h"
#include "sh/net/TimerId.h"

#include <sys/timerfd.h>
#include <unistd.h>


namespace sh
{

namespace net
{

namespace detail
{

int createTimerfd()
{
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                   TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0)
    {
        LOG_SYSFATAL << "Failed in timerfd_create";
    }
    return timerfd;
}

struct timespec howMuchTimeFromNow(TimeStamp when)
{
    int64_t microseconds = when.microSecondsSinceEpoch()
                            - TimeStamp::now().microSecondsSinceEpoch();
    if (microseconds < 100)
    {
        microseconds = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(
                microseconds / TimeStamp::kMicroSecondPerSecond);
    ts.tv_nsec = static_cast<long>(
                 (microseconds % TimeStamp::kMicroSecondPerSecond) * 1000);
    return ts;
}

void readTimerfd(int timerfd, TimeStamp now)
{
    uint64_t howmany; // 超时次数
    ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
    LOG_TRACE << "TimerQueue::handleRead() " << howmany << " at utc+0 " << now.toString();
    if (n != sizeof howmany)
    {
        LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
    }
}

void resetTimerfd(int timerfd, TimeStamp expiration)
{
    // 设置定时器 timerfd_settime()
    struct itimerspec newValue;
    struct itimerspec oldValue;
    memZero(&newValue, sizeof newValue);
    memZero(&oldValue, sizeof oldValue);
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret)
    {
        LOG_SYSERR << "timerfd_settime()";
    }
}

} // namespace detail


TimerQueue::TimerQueue(EventLoop *loop)
    : loop_(loop),
      timerfd_(detail::createTimerfd()),
      timerfdChannel_(loop, timerfd_),
      timers_(),
      callingExpiredTimers_(false)
{
    timerfdChannel_.setReadCallback(
                std::bind(&TimerQueue::handleRead, this));

    // we are always reading the timerfd, we disarm it with timerfd_settime.
    timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
    timerfdChannel_.disableAll();
    timerfdChannel_.remove();
    ::close(timerfd_);
    // do not remove channel, since we are in EventLoop::dtor()
    for (const Entry &timer : timers_) // c++11
    {
        delete timer.second;
    }
}

TimerId TimerQueue::addTimer(TimerCallback cb, TimeStamp when, double interval)
{
    Timer *timer = new Timer(std::move(cb), when, interval);
    loop_->runInLoop(
                std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}

void TimerQueue::cancel(TimerId timerId)
{
    loop_->runInLoop(
                std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::addTimerInLoop(Timer *timer)
{
    loop_->assertInLoopThread();
    bool earliestChanged = insert(timer);
    if (earliestChanged)
    {
        detail::resetTimerfd(timerfd_, timer->expiration());
    }
}

void TimerQueue::cancelInLoop(TimerId timerId)
{
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    ActiveTimerSet::iterator it = activeTimers_.find(timer);
    if (it != activeTimers_.end())
    {
        size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
        assert(n == 1); (void)n;
        delete it->first; // FIXME: no delete please
        activeTimers_.erase(it);
    }
    else if (callingExpiredTimers_)
    {
        cancelingTimers_.insert(timer);
    }
    assert(timers_.size() == activeTimers_.size());
}

void TimerQueue::handleRead()
{
    loop_->assertInLoopThread();
    TimeStamp now(TimeStamp::now());
    detail::readTimerfd(timerfd_, now);

    std::vector<Entry> expired = getExpired(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    // safe to callback outside critical section
    for (const Entry &it : expired)
    {
        it.second->run();
    }
    callingExpiredTimers_ = false;

    reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(TimeStamp now)
{
    assert(timers_.size() == activeTimers_.size());
    std::vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator end = timers_.lower_bound(sentry);
    assert(end == timers_.end() || now < end->first);
    std::copy(timers_.begin(), end, back_inserter(expired));
    timers_.erase(timers_.begin(), end);

    for (const Entry &it : expired)
    {
        ActiveTimer timer(it.second, it.second->sequence());
        size_t n = activeTimers_.erase(timer);
        assert(n == 1); (void)n;
    }

    assert(timers_.size() == activeTimers_.size());
    return expired;
}

void TimerQueue::reset(const std::vector<TimerQueue::Entry> &expired, TimeStamp now)
{
    TimeStamp nextExpire;

    for (const Entry &it : expired)
    {
        ActiveTimer timer(it.second, it.second->sequence());
        if (it.second->repeat()
            && cancelingTimers_.find(timer) == cancelingTimers_.end())
        {
            it.second->restart(now);
            insert(it.second);
        }
        else
        {
            // FIXME move to a free list
            delete it.second; // FIXME no delete please
        }
    }

    if (!timers_.empty())
    {
        nextExpire = timers_.begin()->second->expiration();
    }

    if (nextExpire.valid()) // 有效返回 true
    {
        detail::resetTimerfd(timerfd_, nextExpire);
    }
}

bool TimerQueue::insert(Timer *timer)
{
    loop_->assertInLoopThread();
    assert(timers_.size() == activeTimers_.size());
    bool earliestChanged = false;
    TimeStamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if(it == timers_.end() || when < it->first)
    {
        earliestChanged = true;
    }
    {
        std::pair<TimerList::iterator, bool> result =
                timers_.insert(Entry(when, timer));
        assert(result.second); (void)result;
    }
    {
        std::pair<ActiveTimerSet::iterator, bool> result =
                activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
        assert(result.second); (void)result;
    }

    assert(timers_.size() == activeTimers_.size());
    return earliestChanged;
}



} // namespace net

} // namespace sh





