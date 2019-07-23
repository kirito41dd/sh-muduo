#ifndef SH_BASE_COPYABLE_H
#define SH_BASE_COPYABLE_H

namespace sh
{

// 这个类用来标识一个class是值语义
class copyable
{
protected:
    copyable() = default;
    ~copyable() = default;
};

} // namespace sh

#endif // SH_BASE_COPYABLE_H
