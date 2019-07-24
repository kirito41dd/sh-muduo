#ifndef SH_BASE_EXCEPTION_H
#define SH_BASE_EXCEPTION_H

#include "sh/base/Types.h"
#include <exception>

namespace sh
{

class Exception : public std::exception
{
public:
    Exception(string what);
    ~Exception() noexcept override = default; //如果派生类在虚函数声明时使用了override描述符
                                              // 那么该函数必须重载其基类中的同名函数，否则代码将无法通过编译。
    // 使用默认的拷贝和赋值 , 默认拷贝毁掉用成员的operator=

    const char* what() const noexcept override
    {
        return message_.c_str();
    }

    const char* stackTrace() const noexcept
    {
        return stack_.c_str();
    }

private:
    string message_;
    string stack_;
};


} // namespace sh

#endif // SH_BASE_EXCEPTION_H
