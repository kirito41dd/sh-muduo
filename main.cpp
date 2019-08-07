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
    if(conn->connected())
        cout<< "newconn: "<< conn->peerAddress().toIpPort() <<endl;
    else
        cout<< "disconn: "<< conn->peerAddress().toIpPort() <<endl;
}
void newmessage(const TcpConnectionPtr & conn, Buffer * buf, TimeStamp t)
{
    string m = buf->retrieveAllAsString();
    string mm;
    for(int i = 0; i < m.length(); ++i)
    {
        if(m[i] =='\r' ) mm+="\\r";
        else if(m[i] == '\n') mm+="\\n";
        else mm+=m[i];
    }
    mm = "[" + mm + "]";
    cout<<conn->peerAddress().toIpPort()<<" "
        <<TimeStamp::localNow().toFormattedString(false)
        <<" new message: "<< mm <<endl;
    conn->send("Chong+ Test Server : " + TimeStamp::localNow().toFormattedString(false) + ": " + mm +"\r\n");
}

/*void threadinit(EventLoop*){
    cout<<"threadinit"<<endl;
}*/

/*void writed(const TcpConnectionPtr & conn){
    cout<<"writed"<<endl;
}*/

/*void cb(){
    cout <<"test "<<endl;
}*/

int main(){
    Logger::setLogLevel(Logger::DEBUG);
    InetAddress listenAddr("0.0.0.0",9999);
    EventLoop loop;
    TcpServer server(&loop,listenAddr,"first server");
    server.setConnectionCallback(newconn);
    server.setMessageCallback(newmessage);
    //server.setThreadNum(3);
    //server.setThreadInitCallback(threadinit);
    //server.setWriteCompleteCallback(writed);
    server.start();
    //loop.runEvery(5,cb);
    loop.loop();
    return 0;
}
