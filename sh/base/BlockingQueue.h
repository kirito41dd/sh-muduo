#ifndef SH_BASE_BlockingQueue_H
#define SH_BASE_BlockingQueue_H

#include "sh/base/Condition.h"
#include "sh/base/Mutex.h"

#include <deque>
#include <assert.h>

namespace sh
{

template<typename T>
class BlockingQueue : noncopyable
{
public:
    BlockingQueue()
        : mutex_(),
          notEmpty_(mutex_),
          queue_()
    {
    }

    void put(const T &x)
    {
        MutexLockGuard lock(mutex_);
        queue_.push_back(x);
        notEmpty_.notify();
    }

    void put(T &&x)
    {
        MutexLockGuard lock(mutex_);
        queue_.push_back(std::move(x));
        notEmpty_.notify();
    }

    T take()
    {
        MutexLockGuard lock(mutex_);
        while (queue_.empty())
        {
            notEmpty_.wait();
        }
        assert(!queue_.empty());
        T front(std::move(queue_.front()));
        queue_.pop_front();
        return std::move(front);
    }

    size_t size() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.size();
    }


private:
    mutable MutexLock   mutex_;
    Condition           notEmpty_;
    std::deque<T>       queue_;
}; // class BlockingQueue


} // namespace sh

#endif // SH_BASE_BlockingQueue_H
