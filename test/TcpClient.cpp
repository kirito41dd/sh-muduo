#include <iostream>
#include "sh/net/TcpClient.h"
#include "sh/net/EventLoop.h"
#include "sh/net/InetAddress.h"

using namespace sh;
using namespace sh::net;
using namespace std;
//void (const TcpConnectionPtr &, Buffer *, TimeStamp)
void conncb(const TcpConnectionPtr &ptr)
{
    if (ptr->connected())
    {
        ptr->send("hellow");
    }
}
void newmess(const TcpConnectionPtr & ptr, Buffer * buf, TimeStamp time)
{
    cout<< buf->retrieveAllAsString() <<endl;
}
int main()
{
    EventLoop loop;
    InetAddress serverAddr("127.0.0.1", 9999);
    TcpClient client(&loop, serverAddr, "test_client");
    //client.connection()->send("hellow");
    client.setMessageCallback(newmess);
    client.setConnectionCallback(conncb);
    //client.setWriteCompleteCallback();
    client.connect();
    loop.loop();
    return 0;
}
