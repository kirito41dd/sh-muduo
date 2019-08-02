
#ifndef SH_NET_TIMER_H
#define SH_NET_TIMER_H

#include "sh/base/Atomic.h"
#include "sh/base/TimeStamp.h"
#include "sh/net/Callbacks.h"

namespace sh
{

namespace net
{

/// Internal class for timer event.
class Timer : noncopyable
{
public:

    Timer(TimerCallback cb, TimeStamp when, double interval)
        : callback_(std::move(cb)),
          expiration_(when),
          interval_(interval),
          repeat_(interval > 0.0),
          sequence_(s_numCreated_.incrementAndGet())
    { }

    void run() const
    {
        callback_();
    }

    TimeStamp expiration() const { return expiration_; }
    bool repeat() const { return repeat_; }
    int64_t sequence() const { return sequence_; }

    void restart(TimeStamp now);

    static int64_t numCreated() { return s_numCreated_.get(); }

private:
    const TimerCallback     callback_;
    TimeStamp               expiration_; //  截止,到期
    const double            interval_; // 间隔
    const bool              repeat_;
    const int64_t           sequence_;

    static AtomicInt64      s_numCreated_;
}; // class Timer


} // namespace net

} // namespace sh

#endif // SH_NET_TIMER_H
