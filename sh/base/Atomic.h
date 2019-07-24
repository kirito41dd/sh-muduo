#ifndef SH_BASE_ATOMIC_H
#define SH_BASE_ATOMIC_H

#include "sh/base/noncopyable.h"

#include <stdint.h>


namespace sh
{
namespace detail
{

template<typename T>
class AtomicIntegerT : noncopyable
{
public:
    AtomicIntegerT()
        : value_(0)
    {
    }

    // uncomment if you need copying and assignment
    //
    // AtomicIntegerT(const AtomicIntegerT& that)
    //   : value_(that.get())
    // {}
    //
    // AtomicIntegerT& operator=(const AtomicIntegerT& that)
    // {
    //   getAndSet(that.get());
    //   return *this;
    // }

    T get()
    {
        // in gcc >= 4.7: __atomic_load_n(&value_, __ATOMIC_SEQ_CST)
        return __sync_val_compare_and_swap(&value_, 0, 0);
    }

    T getAndAdd(T x)
    {
      // in gcc >= 4.7: __atomic_fetch_add(&value_, x, __ATOMIC_SEQ_CST)
      return __sync_fetch_and_add(&value_, x);
    }

     T addAndGet(T x)
     {
       return getAndAdd(x) + x;
     }

     T incrementAndGet()
     {
       return addAndGet(1);
     }

     T decrementAndGet()
     {
       return addAndGet(-1);
     }

     void add(T x)
     {
       getAndAdd(x);
     }

     void increment()
     {
       incrementAndGet();
     }

     void decrement()
     {
       decrementAndGet();
     }

     T getAndSet(T newValue)
     {
       // in gcc >= 4.7: __atomic_exchange_n(&value, newValue, __ATOMIC_SEQ_CST)
       return __sync_lock_test_and_set(&value_, newValue);
     }

private:
    volatile T value_;
    // volatile 作为指令关键字，确保本条指令不会因编译器的优化而省略，且要求每次直接读值。
    // 防止编译器对代码进行优化。系统总是重新从它所在的内存读取数据，而不是使用保存在寄存器中的备份。

}; //class AtomicIntegerT

} // namespace detail

typedef detail::AtomicIntegerT<int32_t> AtomicInt32;
typedef detail::AtomicIntegerT<int64_t> AtomicInt64;

} // namespace sh

#endif // SH_BASE_ATOMIC_H

/*
 gcc从4.1.2提供了__sync_*系列的built-in函数，用于提供加减和逻辑运算的原子操作。
    type __sync_fetch_and_add (type *ptr, type value, ...)
    type __sync_fetch_and_sub (type *ptr, type value, ...)
    type __sync_fetch_and_or (type *ptr, type value, ...)
    type __sync_fetch_and_and (type *ptr, type value, ...)
    type __sync_fetch_and_xor (type *ptr, type value, ...)
    type __sync_fetch_and_nand (type *ptr, type value, ...)


    type __sync_add_and_fetch (type *ptr, type value, ...)
    type __sync_sub_and_fetch (type *ptr, type value, ...)
    type __sync_or_and_fetch (type *ptr, type value, ...)
    type __sync_and_and_fetch (type *ptr, type value, ...)
    type __sync_xor_and_fetch (type *ptr, type value, ...)
    type __sync_nand_and_fetch (type *ptr, type value, ...)
 这两组函数的区别在于第一组返回更新前的值，第二组返回更新后的值。
 type可以是1,2,4或8字节长度的int类型，即：
    int8_t / uint8_t
    int16_t / uint16_t
    int32_t / uint32_t
    int64_t / uint64_t
 后面的可扩展参数(...)用来指出哪些变量需要memory barrier,因为目前gcc实现的是full barrier
（类似于linux kernel 中的mb(),表示这个操作之前的所有内存操作不会被重排序到这个操作之后）所以可以略掉这个参数。


    bool __sync_bool_compare_and_swap (type *ptr, type oldval type newval, ...)
    type __sync_val_compare_and_swap (type *ptr, type oldval type newval, ...)
这两个函数提供原子的比较和交换，如果*ptr == oldval,就将newval写入*ptr,
第一个函数在相等并写入的情况下返回true.
第二个函数在返回操作之前的值。


type __sync_lock_test_and_set (type *ptr, type value, ...)
   将*ptr设为value并返回*ptr操作之前的值。

void __sync_lock_release (type *ptr, ...)
     将*ptr置0

https://www.cnblogs.com/FrankTan/archive/2010/12/11/1903377.html
*/






