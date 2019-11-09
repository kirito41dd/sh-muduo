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
