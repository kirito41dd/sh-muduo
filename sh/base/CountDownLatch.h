// 可以替代 pthread 的 屏障（barrier）

#ifndef SH_BASE_COUNTDOWNLATCH_H
#define SH_BASE_COUNTDOWNLATCH_H

#include "sh/base/Condition.h"
#include "sh/base/Mutex.h"

namespace sh
{

class CountDownLatch : noncopyable
{
public:

    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

private:
    // mutable也是为了突破const的限制而设置的。被mutable修饰的变量，将永远处于可变的状态，即使在一个const函数中。
    mutable MutexLock   mutex_;
    Condition           condition_;
    int                 count_;
}; // class CountDownLatch

} // namespace sh

#endif // SH_BASE_COUNTDOWNLATCH_H
