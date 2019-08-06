#include "sh/net/TcpServer.h"
#include "sh/net/EventLoop.h"
#include "sh/base/Logging.h"
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace sh;
using namespace sh::net;

void newconn(const TcpConnectionPtr & conn)
{
    cout<< "newconn: "<<conn->getTcpInfoString() <<endl;
}
void newmessage(const TcpConnectionPtr & conn, Buffer * buf, TimeStamp t)
{
    cout<<"new message: "<< buf->retrieveAllAsString()<<endl;
    conn->send(TimeStamp::localNow().toString());
}

void threadinit(EventLoop*){
    cout<<"threadinit"<<endl;
}

void writed(const TcpConnectionPtr &){
    cout<<"writed"<<endl;
}

void cb(){
    cout <<"test "<<endl;
}

int main(){
    Logger::setLogLevel(Logger::DEBUG);
    InetAddress listenAddr("0.0.0.0",9999);
    EventLoop loop;
    TcpServer server(&loop,listenAddr,"first server");
    server.setConnectionCallback(newconn);
    server.setMessageCallback(newmessage);
    //server.setThreadNum(3);
    server.setThreadInitCallback(threadinit);
    server.setWriteCompleteCallback(writed);
    server.start();
    //loop.runEvery(5,cb);
    loop.loop();
    return 0;
}
