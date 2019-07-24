// 这个头文件包装了互斥锁 以及管理锁的类
// MutexLock MutexLockGuard


#ifndef SH_BASE_MUTEX_H
#define SH_BASE_MUTEX_H

#include "sh/base/CurrentThread.h"
#include "sh/base/noncopyable.h"

#include <assert.h>
#include <pthread.h>

namespace sh
{

class MutexLock : noncopyable
{
public:
    MutexLock()
        : holder_(0)
    {
        int ret = pthread_mutex_init(&mutex_, NULL);
        assert(ret == 0); // 返回0表示成功
    }

    ~MutexLock()
    {
        assert(holder_ == 0);
        int ret = pthread_mutex_destroy(&mutex_);
        assert(ret == 0);
    }

    // 必须在锁定后才能调研
    bool isLockedByThisThread() const
    {
        return holder_ == CurrentThread::tid();
    }

    void assertLocked() const
    {
        assert(isLockedByThisThread());
    }

    // 核心使用
    void lock()
    {
        int ret = pthread_mutex_lock(&mutex_);
        assert(ret == 0);
        assignHolder();
    }

    void unlock()
    {
        unassignHolder();
        int ret = pthread_mutex_unlock(&mutex_);
        assert(ret == 0);
    }

    pthread_mutex_t* getPthreadMutex() /* non-const */
    {
        return &mutex_;
    }

private:
    friend class Condition; // 条件变量需要用锁
    class UnassignGuard : noncopyable // assign 分配 派遣  这个类也是给条件变量准备的
    {
    public:
        explicit UnassignGuard(MutexLock &owner)
            : owner_(owner)
        {
            owner_.unassignHolder();
        }

        ~UnassignGuard()
        {
            owner_.assignHolder();
        }

    private:
        MutexLock &owner_;
    }; // class unassignGuard

    void unassignHolder()
    {
        holder_ = 0;
    }
    void assignHolder()
    {
        holder_ = CurrentThread::tid();
    }

    pthread_mutex_t mutex_;
    pid_t           holder_;
}; // class MutexLock


// 用栈上的对象来负责加锁解锁
class MutexLockGuard : noncopyable
{
public:
    explicit MutexLockGuard(MutexLock &mutex)
        : mutex_(mutex)
    {
        mutex_.lock();
    }

    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

private:
    MutexLock &mutex_;
}; // class MutexLockGuard

} // namespace sh

#endif // SH_BASE_MUTEX_H
