#ifndef SH_BASE_NONCOPYABLE_H
#define SH_BASE_NONCOPYABLE_H

namespace sh
{

// 继承这个基类，表示对象语义，不能复制，相对于值语义
class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;       // 不提供拷贝构造器和赋值操作
    void operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

} // namespace sh

#endif // SH_BASE_NONCOPYABLE_H
