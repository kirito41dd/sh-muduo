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
private:
    // FIXME: 使用unique_ptr<Timer>代替源生指针
    typedef std::pair<TimeStamp, Timer *>   Entry;
    typedef std::set<Entry>                 TimerList;
    typedef std::pair<Timer *, int64_t>     ActiveTimer;
    typedef std::set<ActiveTimer>           ActiveTimerSet;



    EventLoop *loop_;
    const int timerfd_;
    Channel timerfdChannel_;

    // 小顶堆，时间截止的时间优先
    TimerList timers_;

    // for cancel()
    ActiveTimerSet activeTimers_;
    bool           callingExpiredTimers_;
    ActiveTimerSet cancelingTimers_;

}; // class TimerQueue



} // namespace net

} // namespace sh

#endif // SH_NET_TIMERQUEUE_H
