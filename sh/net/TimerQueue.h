// 内部使用的头文件

#ifndef SH_NET_TIMERQUEUE_H
#define SH_NET_TIMERQUEUE_H

#include "sh/base/Mutex.h"
#include "sh/base/TimeStamp.h"
#include "sh/net/Callbacks.h"
#include "sh/net/Channel.h"

#include <set>
#include <vector>

namespace sh
{

namespace net
{

class EventLoop;
class Timer;
class TimerId;

/// 不保证回调能准时调用
/// 一个尽力准时的队列
class TimerQueue : noncopyable
{
public:
    explicit TimerQueue(EventLoop *loop);
    ~TimerQueue();

    /// 在给定的时间调用回调
    /// 如果@c interval > 0.0 则重复
    /// 必须是线程安全的，通常被其他线程调用
    TimerId addTimer(TimerCallback cb, TimeStamp when, double interval);

    void cancel(TimerId timerId);

private:
    // FIXME: 使用unique_ptr<Timer>代替源生指针
    typedef std::pair<TimeStamp, Timer *>   Entry;
    typedef std::set<Entry>                 TimerList;
    typedef std::pair<Timer *, int64_t>     ActiveTimer;
    typedef std::set<ActiveTimer>           ActiveTimerSet;

    void addTimerInLoop(Timer *timer);
    void cancelInLoop(TimerId timerId);
    // called when timerfd alarms
    void handleRead();
    // move out all expired timers
    std::vector<Entry> getExpired(TimeStamp now);
    void reset(const std::vector<Entry> &expired, TimeStamp now);

    bool insert(Timer *timer);


    EventLoop *loop_;
    const int timerfd_;
    Channel timerfdChannel_;

    // 小顶堆，时间截止的时间优先
    TimerList timers_;  // 与activeTimers_ 内容总是一致，timers_负责排序

    // for cancel()
    ActiveTimerSet activeTimers_;
    bool           callingExpiredTimers_;
    ActiveTimerSet cancelingTimers_;

}; // class TimerQueue



} // namespace net

} // namespace sh

#endif // SH_NET_TIMERQUEUE_H
