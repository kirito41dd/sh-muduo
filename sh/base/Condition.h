// 包装条件变量
// 一般作底层，不直接使用

#ifndef SH_BASE_CONDITION_H
#define SH_BASE_CONDITION_H

#include "sh/base/Mutex.h"

#include <pthread.h>

namespace sh
{

class Condition : noncopyable
{
public:
    explicit Condition(MutexLock &mutex)
        :mutex_(mutex)
    {
        int ret = pthread_cond_init(&pcond_, NULL);
        assert(ret == 0);
    }

    ~Condition()
    {
        int ret = pthread_cond_destroy(&pcond_);
    }

    void wait()
    {
        MutexLock::UnassignGuard ug(mutex_); // 等待时候线程不持有锁
        int ret = pthread_cond_wait(&pcond_, mutex_.getPthreadMutex()); // 函数把线程放到等待条件变量的列表上，
        assert(ret == 0);                                     // 然后解锁互斥量。休眠等待条件返回时互斥量再次被锁住。
    }

    // 等待 超时立即返回
    // return true if timeout, false otherwise.
    bool waitForSeconds(double seconds);

    void notify()
    {
        int ret = pthread_cond_signal(&pcond_);
        assert(ret == 0);
    }

    void notifyAll()
    {
        int ret = pthread_cond_broadcast(&pcond_);
        assert(ret == 0);
    }

private:
    MutexLock       &mutex_;
    pthread_cond_t  pcond_;
}; // class Condition

} // namespace sh

#endif // SH_BASE_CONDITION_H
