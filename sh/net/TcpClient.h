#ifndef SH_NET_TCPCLIENT_H
#define SH_NET_TCPCLIENT_H

#include "sh/base/Mutex.h"
#include "sh/net/TcpConnection.h"

namespace sh
{

namespace net
{

class Connector;
typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient : noncopyable
{
public:
    TcpClient(EventLoop *loop,
              const InetAddress &serverAddr,
              const string &name);
    ~TcpClient(); // force out-line dtor, for std::unique_ptr members.

    void connect();
    void disconnect();
    void stop();

    TcpConnectionPtr connection() const
    {
        MutexLockGuard lock(mutex_);
        return connection_;
    }

    EventLoop* getLoop() const { return loop_; }
    bool retry() const { return retry_; }
    void enableRetry() { retry_ = true; }

    const string& name() const { return name_; }

    /// Set connection callback.
    /// Not thread safe.
    void setConnectionCallback(ConnectionCallback &cb)
    { connectionCallback_ = cb; }

    /// Set message callback.
    /// Not thread safe.
    void setMessageCallback(MessageCallback &cb)
    { messageCallback_ = cb; }

    /// Set write complete callback.
    /// Not thread safe.
    void setWriteCompleteCallback(WriteCompleteCallback &cb)
    { WriteCompleteCallback_ = cb; }

private:
    /// not thread safe, but in loop
    void newConnection(int sockfd);
    /// not thread safe, but in loop
    void removeConnection(const TcpConnectionPtr &conn);

    EventLoop *loop_;
    ConnectorPtr connector_; // 不要暴露connector
    const string name_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback WriteCompleteCallback_;
    bool retry_; // atomic
    bool connect_; // atomic
    // always in loop thread
    int nextConnId_;
    mutable MutexLock mutex_;
    TcpConnectionPtr connection_; // GUARDED BY mutex

}; // class TcpClient

} // namespace net

} // namespace sh

#endif // SH_NET_TCPCLIENT_H
