// test sh/base/Singleton.h
// 单例模式测试
// Author: zsh 1026860069@qq.com

#include <sh/base/Singleton.h>
#include <iostream>

class A
{
public:
    A()
    {
        std::cout << "A()" << std::endl;
    }

    void init()
    {
        std::cout << "I am A" << std::endl;
    }

    ~A()
    {
        std::cout << "~A()" << std::endl;
    }
};

int main()
{
    sh::Singleton<A>::instance().init();
    return 0;
}
