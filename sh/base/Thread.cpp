
#include "sh/base/Thread.h"
#include "sh/base/CurrentThread.h"
#include "sh/base/Exception.h"
#include "sh/base/Logging.h"

#include <type_traits>

#include <unistd.h>  // syscall
#include <errno.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/syscall.h> // SYS_gettid
#include <sys/types.h>
#include <linux/unistd.h>

namespace sh
{
namespace detail
{

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void afterFork()
{
    CurrentThread::t_cachedTid = 0;
    CurrentThread::t_threadName = "main";
    CurrentThread::tid();
    // 不需要调用 pthread_atfork(NULL, NULL, &afterFork);
}

class ThreadNameInitializer
{
public:
    ThreadNameInitializer()
    {
        CurrentThread::t_threadName = "main";
        CurrentThread::tid();
        pthread_atfork(NULL, NULL, &afterFork); // 安装清理函数 fork 前  fork调用返回前在父进程上下文  fork后子进程 中调用
    }
}; // class ThreadNameInitializer

struct ThreadData
{
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc      func_;
    string          name_;
    pid_t           *tid_;
    CountDownLatch  *latch_;

    ThreadData(ThreadFunc func, const string &name, pid_t *tid, CountDownLatch *latch)
        : func_(std::move(func)),
          name_(name),
          tid_(tid),
          latch_(latch)
    { }

    void runInThread()
    {
        *tid_ = CurrentThread::tid();
        tid_ = NULL;
        latch_->countDown();  // Thread::start() 收到信号返回
        latch_ = NULL;

        CurrentThread::t_threadName = name_.empty() ? "shThread" : name_.c_str();
        // 使用 prctl(2) ，可以对与系统上的活动进程、任务或项目关联的资源控制进行运行时询问和修改。
        ::prctl(PR_SET_NAME, CurrentThread::t_threadName); // 设置进/线程名字
        try {
            func_();
            CurrentThread::t_threadName = "finished";
        } catch (const Exception &ex) {
            CurrentThread::t_threadName = "crashed";
            fprintf(stderr,"exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            fprintf(stderr, "stack trace: %s\n", ex.stackTrace());
            abort();
        } catch (const std::exception &ex) {
            CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            abort();
        } catch (...) {
            CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
            throw; // rethrow
        }
    }
}; // struct ThreadData

void* startThread(void *obj)
{
    ThreadData *data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

} // namespace detail

void CurrentThread::cacheTid()  // CurrentThread.h
{
    if (t_cachedTid == 0)
    {
        t_cachedTid = detail::gettid();
        t_tidStringLenght = snprintf(t_tidString, sizeof t_tidString, "%5d", t_cachedTid);
    }
}

bool CurrentThread::isMainThread()
{
    return tid() == ::getpid();
}

void CurrentThread::sleepUsec(int64_t usec)
{
    struct timespec ts = {0, 0}; // 秒 纳秒
    ts.tv_sec = static_cast<time_t>(usec / TimeStamp::kMicroSecondPerSecond);
    ts.tv_nsec = static_cast<long>(usec % TimeStamp::kMicroSecondPerSecond * 1000);
    ::nanosleep(&ts, NULL);
}

AtomicInt32 Thread::numCreated_;

Thread::Thread(Thread::ThreadFunc func, const std::string &name)
    : started_(false),
      joined_(false),
      pthreadID_(0),
      tid_(0),
      func_(std::move(func)),
      name_(name),
      latch_(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if ( started_ && !joined_)
    {
        pthread_detach(pthreadID_); // 如果Thread析构时线程还在运行,设置其为分离态;
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    // FIXME: move(func_)
    detail::ThreadData* data = new detail::ThreadData(func_, name_, &tid_, &latch_);
    if (pthread_create(&pthreadID_, NULL, &detail::startThread, data)) {
        started_ = false;
        delete data;
        LOG_SYSFATAL << "Failed in pthread_create";
    } else {
        latch_.wait();
        assert(tid_ > 0);
    }
}

int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadID_, NULL);
}

void Thread::setDefaultName()
{
    int num = numCreated_.incrementAndGet();
    if (name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d",num);
        name_ = buf;
    }
}



} // namespace sh
