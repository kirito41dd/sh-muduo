# sh-muduo
学习muduo网络库，试着自己实现一遍

## 记录一下学到的东西
override描述符：虚函数声明时使用了override描述符,派生类必须重载虚函数，否则代码将无法通过编译。`base/Exception.h`

mutable： 突破const的限制  `base/CountDownLatch.h`

条件变量的虚假唤醒：spurious wakeups

__thread： 修饰变量，每一个线程有一份独立实体，各个线程的值互不干扰 `base/CurrentThread.h`

__builtin_expect(EXP, N): gcc引入的，作用是允许程序员将最有可能执行的分支告诉编译器。意思是：EXP==N的概率很大。`base/CurrentThread.h`

gcc从4.1.2提供了__sync_*系列的built-in函数，用于提供加减和逻辑运算的原子操作： `base/Atomic.h`

非类型参数的模板类：C++的标准仅支持整型、枚举、指针类型和引用类型 `base/LogStream.h`

模板实例化(不是特化)： `base/LogStream.h`  `base/LogStream.cpp`

offsetof(TYPE, MEMBER)： 这个宏来测量成员在结构中的便宜 InetAddress.cpp #46

std::enable_shared_from_this<T> : TcpConnection.h
  
const static int knum = 10 ： 在头文件定义类成员，这种只是声明+初始化（仅支持整型bool也算），没有定义，不能用使用引用传参（因为没有分配内存，但可以传值）。使用引用必须在.cpp文件定义
