#ifndef SH_BASE_CURRENTTHREAD_H
#define SH_BASE_CURRENTTHREAD_H

#include "sh/base/Types.h"

namespace sh
{
namespace CurrentThread
{
    // __thread是GCC内置的线程局部存储设施，存取效率可以和全局变量相比。
    // __thread变量每一个线程有一份独立实体，各个线程的值互不干扰。
    // 可以用来修饰那些带有全局性且值可能变，但是又不值得用全局变量保护的变量。
    // internal
    extern __thread int         t_cachedTid;
    extern __thread char        t_tidString[32];
    extern __thread int         t_tidStringLenght;
    extern __thread const char  *t_threadName;
    void cacheTid(); // 定义在 Thread.cpp

    inline int tid()
    {
        // 这个指令是gcc引入的，作用是允许程序员将最有可能执行的分支告诉编译器。
        // 指令的写法为：__builtin_expect(EXP, N),意思是：EXP==N的概率很大。
        if (__builtin_expect(t_cachedTid == 0, 0)){
            cacheTid();
        }
        return t_cachedTid;
    }

    inline const char* tidString() // for logging
    {
        return t_tidString;
    }

    inline int tidStringLength() // for logging
    {
        return t_tidStringLenght;
    }

    inline const char* name()
    {
        return t_threadName;
    }

    bool isMainThread();

    void sleepUsec(int64_t usec); // for testing

    // 将C++源程序标识符(original C++ source identifier)转换成C++ ABI标识符(C++ ABI identifier)的过程
    // 称为mangle；相反的过程称为demangle。
    string stackTrace(bool demangle);

} // namespace CurrnetThread

} // namespace sh

#endif // SH_BASE_CURRENTTHREAD_H
