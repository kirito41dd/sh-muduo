
#ifndef SH_NET_TIMERID_H
#define SH_NET_TIMERID_H

#include "sh/base/copyable.h"
#include "sh/base/Types.h"

namespace sh
{

namespace net
{

class Timer;

class TimerId : copyable
{
public:

    TimerId()
        : timer_(NULL),
          sequence_(0)
    { }

    TimerId(Timer *timer, int64_t seq)
        : timer_(timer),
          sequence_(seq)
    { }

    // 使用默认拷贝，赋值

    friend class TimerQueue;

private:
    Timer   *timer_;
    int64_t sequence_;
}; // class TimerId



} // namespace net

} // namespace sh

#endif // SH_NET_TIMERID_H
