# sh-muduo
[![](https://img.shields.io/github/stars/zshorz/sh-muduo?style=social)](https://github.com/zshorz/sh-muduo/stargazers) [![](https://img.shields.io/github/languages/top/zshorz/sh-muduo?style=plastic)](https://github.com/zshorz/sh-muduo) [![](https://img.shields.io/github/last-commit/zshorz/sh-muduo)](https://github.com/zshorz/sh-muduo/commits/master)

学习muduo网络库，试着自己实现一遍，欢迎给陈硕大神的项目打星[https://github.com/chenshuo/muduo](https://github.com/chenshuo/muduo)

安装：
```bash
git clone https://github.com/zshorz/sh-muduo.git sh-muduo
mkdir build && cd build
cmake ../sh-muduo/   # install 默认安装到 /usr/local/ 或设置 CMAKE_INSTALL_PREFIX
make
sudo make install
```
由于动态库默认的搜索路径是 /usr/lib 和 /lib

可以吧 /usr/local/lib/ 下的 libsh_base.so libsh_net.so 创建链接
```bash
sudo ln -s /usr/local/lib/libsh_*.so /usr/lib
sudo ldconfig
```
或者使用root用户
```bash
echo "/usr/local/lib" >> /etc/ld.so.conf
ldconfig
```

一个简单的例子： test.cpp
```cpp
#include <iostream>
#include <sh/net/EventLoop.h>

using namespace std;

void cb()
{
    cout << "hello world" << endl;
}

int main()
{
    sh::net::EventLoop loop;
    loop.runEvery(1,cb);
    loop.loop();
    return 0;
}

```
```bash
g++ test.cpp -lsh_net -o test
./test
hellow
hellow
^C
```


## 记录一下学到的东西
override描述符：虚函数声明时使用了override描述符,派生类必须重载虚函数，否则代码将无法通过编译。`base/Exception.h`

mutable： 突破const的限制  `base/CountDownLatch.h`

条件变量的虚假唤醒：spurious wakeups

__thread： 修饰变量，每一个线程有一份独立实体，各个线程的值互不干扰 `base/CurrentThread.h`

__builtin_expect(EXP, N): gcc引入的，作用是允许程序员将最有可能执行的分支告诉编译器。意思是：EXP==N的概率很大。`base/CurrentThread.h`

gcc从4.1.2提供了__sync_*系列的built-in函数，用于提供加减和逻辑运算的原子操作： `base/Atomic.h`

非类型参数的模板类：C++的标准仅支持整型、枚举、指针类型和引用类型 `base/LogStream.h`

模板实例化(不是特化)： `base/LogStream.h`  `base/LogStream.cpp`

offsetof(TYPE, MEMBER)： 这个宏来测量成员在结构中的偏移 InetAddress.cpp #46

std::enable_shared_from_this<T> : TcpConnection.h

const static int knum = 10 ： 在头文件定义类成员，这种只是声明+初始化（仅支持整型bool也算），没有定义，不能用使用引用传参（因为没有分配内存，但可以传值）。使用引用必须在.cpp文件定义
