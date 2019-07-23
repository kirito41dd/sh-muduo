#ifndef SH_BASE_TYPES_H
#define SH_BASE_TYPES_H

#include <stdint.h>
#include <string.h>     // memset
#include <string>

#ifndef NDEBUG
#include <assert.h>
#endif

namespace sh
{

using std::string;  // sh::string

inline void memZero(void *p, size_t n)
{
    memset(p,0,n);
}

// 可以在任何可以用static_cast的地方用implicit_cast
// implicit_cast 只能执行up-cast，如派生类到基类的转换
// static_cast 可以执行up-cast和down-cast，而把基类转换为派生类应该是不被允许的，static_cast可通过编译,不安全
template<typename To, typename From>
inline To implicit_cast(From const &f)
{
    return f;
}

// 如果是up-cast,最好使用implicit_cast
// 当你想要down-cast，static_cast是不安全的
// dynamic_cast,将一个基类对象指针（或引用）cast到继承类指针，dynamic_cast会根据基类指针是否真正指向继承类指针来做相应处理
// 但凡程序设计正确，dynamic_cast就可以用static_cast来替换，而后者比前者更有效率。
// 在你down-cast时候，年
template<typename To, typename From>
inline To down_cast(From *f)    // 只接受指针 down_cast<T*>(foo);
{
#if !defined(NDEBUG)
    assert(f == NULL || dynamic_cast<To>(f) != NULL);  //在debug模式下用dynamic_cast检查
#endif
    return static_cast<To>(f);
}


} // namespace sh


#endif // SH_BASE_TYPES_H
