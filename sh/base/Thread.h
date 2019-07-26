#ifndef SH_BASE_THREAD_H
#define SH_BASE_THREAD_H

#include "sh/base/Atomic.h"
#include "sh/base/CountDownLatch.h"
#include "sh/base/Types.h"

#include <functional>
#include <memory>
#include <pthread.h>

namespace sh
{

class Thread : noncopyable
{
public:
    typedef std::function<void ()> ThreadFunc;

    explicit Thread(ThreadFunc func, const string &name = string());

    ~Thread();

    void start(); // 启动线程
    int join(); // return pthread_join() 等待线程返回

    bool started() const { return started_; }
    // pthread_t pthreadID const { return pthreadID_; }
    pid_t tid() const { return tid_; }
    const string& name() const { return name_; }
    static int numCreated() { return numCreated_.get(); }

private:
    void setDefaultName();

    bool            started_;
    bool            joined_;
    pthread_t       pthreadID_;
    pid_t           tid_;
    ThreadFunc      func_;
    string          name_;
    CountDownLatch  latch_;

    static AtomicInt32 numCreated_;
}; // class Thread

} // namespace sh

#endif // SH_BASE_THREAD_H
