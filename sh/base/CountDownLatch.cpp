
#include "sh/base/CountDownLatch.h"

using namespace sh;


CountDownLatch::CountDownLatch(int count)
    : mutex_(),
      condition_(mutex_),
      count_(count)
{
}

void CountDownLatch::countDown()
{
    MutexLockGuard lock(mutex_);
    --count_;
    if (count_ == 0)
    {
        condition_.notifyAll();
    }
}

void CountDownLatch::wait()
{
    MutexLockGuard lock(mutex_);
    while (count_ > 0) // 防止 spurious wakeups 虚假唤醒
    {
        condition_.wait();
    }
}

int CountDownLatch::getCount() const
{
    MutexLockGuard lock(mutex_);
    return count_;
}
