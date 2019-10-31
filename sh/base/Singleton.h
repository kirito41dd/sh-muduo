// 单例模式 没有使用pthread.h
// 头文件可以单独使用

#ifndef SH_BASE_SINGLETON_H
#define SH_BASE_SINGLETON_H

#include <sh/base/noncopyable.h>
#include <stdlib.h> // for atexit
#include <pthread.h>
#include <assert.h>

namespace sh
{

template<typename T>
class Singleton : sh::noncopyable
{
public:
    Singleton() = delete;
    ~Singleton() = delete;

    static T& instance()
    {
        ::pthread_once(&ponce_, &Singleton::init);

        assert(value_ != NULL);
        return *value_;
    }

private:
    static void init()
    {
        value_ = new T();
        ::atexit(&Singleton::destroy);
    }

    static void destroy()
    {
        delete value_;
        value_ = NULL;
    }

    static pthread_once_t   ponce_;
    static T*               value_;
}; // class Singleton

template<typename T>
pthread_once_t Singleton<T>::ponce_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value_ = NULL;

} // namespace sh

#endif // SH_BASE_SINGLETON_H
