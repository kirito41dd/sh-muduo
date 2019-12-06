// 一个简单的 echo 服务器
// 使用 nc 链接测试: nc 127.0.0.1 9999

#include <sh/net/EventLoop.h>
#include <sh/net/TcpServer.h>
#include <sh/base/TimeStamp.h>
#include <iostream>


using namespace sh::net;
using namespace sh;
using namespace std;

void newConn(const TcpConnectionPtr &conn) {
    if (conn->connected()) {
        cout << "new connection: " << conn->peerAddress().toIpPort() << endl;
    } else {
        cout << "disconnect: " << conn->peerAddress().toIpPort() << endl;
    }

}

void newMsg(const TcpConnectionPtr &conn, Buffer *buf, TimeStamp stamp) {
    string data = buf->retrieveAllAsString();
    cout << conn->peerAddress().toIpPort() << " : " << data << endl;
    data = TimeStamp::localNow().toFormattedString() + " " + data;
    conn->send(data);
}

int main() {

    EventLoop loop;
    InetAddress addr("127.0.0.1", 9999);
    TcpServer server(&loop, addr, "echo server");

    server.setConnectionCallback(newConn);
    server.setMessageCallback(newMsg);
    server.start();

    cout << "echo server listen at " << addr.toIpPort() << endl;

    loop.loop();
    return 0;
}
