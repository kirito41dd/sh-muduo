#ifndef SH_NET_HTTP_HTTPSERVER_H
#define SH_NET_HTTP_HTTPSERVER_H

#include "sh/net/TcpServer.h"

namespace sh
{
namespace net
{
namespace http
{
class HttpRequest;
class HttpResponse;

/// 不完全支持 HTTP 1.1 , 只支持部分功能
class HttpServer : noncopyable
{
public:
    typedef std::function<void (const HttpRequest&, HttpResponse*)> HttpCallback;

    HttpServer(EventLoop *loop, const InetAddress &listenAddr,
               const string &name,
               TcpServer::Option option = TcpServer::kNoReusePort);
    ~HttpServer() = default;

    EventLoop* getLoop() const { return server_.getLoop(); }

    /// not thread safe, do it before start()
    void setHttpCallback(const HttpCallback &cb) { httpCallback_ = cb; }
    void setThreadNum(int n) { server_.setThreadNum(n); }

    void start();

private:
    void onConnection(const TcpConnectionPtr &conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer *buf, TimeStamp receiveTime);
    void onRequest(const TcpConnectionPtr& conn, const HttpRequest& req);

    TcpServer       server_;
    HttpCallback    httpCallback_;
}; // class HttpServer

} // namespace http
} // namespace net
} // namespace sh

#endif // SH_NET_HTTP_HTTPSERVER_H
