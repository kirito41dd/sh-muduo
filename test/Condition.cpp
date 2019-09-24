#include <sh/base/Condition.h>
#include <sh/base/Mutex.h>
#include <sh/base/Thread.h>
#include <iostream>

using namespace std;
using namespace sh;

MutexLock mutex;
Condition cond(mutex);

void fun()
{
    cout << "in fun()" << endl;
    cond.notify();
    cout << "notify" << endl;
}

int main()
{
    Thread t(fun, "testThread");
    cout << "start thread" << endl;
    t.start();
    cout << "start sleep" << endl;
    //CurrentThread::sleepUsec(1000*1000*5); // sleep 5 second  如果在wait之前notify，wait收不到这次唤醒
    cout << "end sleep" << endl;
    {
        MutexLockGuard lock(mutex);
        cond.wait();
    }
    cout << "wait return" << endl;
}
