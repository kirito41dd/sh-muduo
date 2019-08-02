#include "sh/net/Timer.h"

namespace sh
{

namespace net
{

AtomicInt64 Timer::s_numCreated_; // static

void Timer::restart(TimeStamp now)
{
    if (repeat_)
    {
        expiration_ = addTime(now, interval_);
    }
    else
    {
        expiration_ = TimeStamp::invalid();
    }
}

} // namespace net

} // namespace sh
