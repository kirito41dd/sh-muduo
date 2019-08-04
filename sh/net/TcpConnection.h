#ifndef SH_NET_TCPCONNECTION_H
#define SH_NET_TCPCONNECTION_H

#include "sh/base/noncopyable.h"
#include "sh/base/StringPiece.h"
#include "sh/base/Types.h"
#include "sh/net/Callbacks.h"
#include "sh/net/InetAddress.h"
#include "sh/net/Buffer.h"

#include <memory>
#include <boost/any.hpp>

struct tcp_info; // <netinet/tcp.h>

namespace sh
{

namespace net
{

class Channel;
class EventLoop;
class Socket;

// 若一个类 T 继承 std::enable_shared_from_this<T> ，
// 则会为该类 T 提供成员函数： shared_from_this 。
// 当 T 类型对象 t 被一个为名为 pt 的 std::shared_ptr<T> 类对象管理时，
// 调用 T::shared_from_this 成员函数，将会返回一个新的 std::shared_ptr<T> 对象，
// 它与 pt 共享 t 的所有权。

// 当类A被share_ptr管理，且在类A的成员函数里需要把当前类对象作为参数传给其他函数时，
// 就需要传递一个指向自身的share_ptr。

/// 用来表示一个tcp连接，接口类，不暴露太多细节
class TcpConnection : noncopyable,
                      std::enable_shared_from_this<TcpConnection>
{
public:
    /// 使用一个 connected sockfd 初始化
    /// 用户不应该创建这个类
    TcpConnection(EventLoop *loop,
                  const string &name,
                  int sockfd,
                  const InetAddress &localAddr,
                  const InetAddress &peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const { return loop_; }
    const string& name() const { return name_; }
    const InetAddress& localAddress() const { return localAddr_; }
    const InetAddress& peerAddress() const { return peerAddr_; }
    bool connected() const {return state_ == kConnected; }
    bool disconnected() const { return state_ == kDisconnected; }
    // return true is success
    bool getTcpInfo(struct tcp_info *info) const;
    string getTcpInfoString() const;

    // void send(string &&message); // c++11
    void send(const void *message, int len);
    void send(const StringPiece &message);
    void send(Buffer *message);
    // void send(Buffer &&message);
    void shutdown(); // 非线程安全 不要同时调用
    // void shutdownAndForceCloseAfter(double seconds); // 非线程安全 不要同时调用
    void forceClose();
    void forceCloseWithDelay(double seconds);
    void setTcpNoDelay(bool on);
    // reading or not
    void startRead();
    void stopRead();
    bool isReading() const { return reading_; } // NOT thread safe, may race with start/stopReadInLoop

    void setContext(const boost::any &context) { context_ = context; }
    const boost::any& getContext() const { return context_; }
    boost::any* getMutableContext() { return &context_; }

    void setConnectionCallback(const ConnectionCallback &cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback &cb)
    { messageCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback &cb)
    { writeCompleteCallback_ = cb; }

    void setHighWaterMarkCallback(const HighWaterMarkCallback &cb)
    { highWaterMarkCallback_ = cb; }

    /// Advaced interface
    Buffer* inputBuffer() { return &inputBuffer_; }
    Buffer* outputBuffer() { return &outputBuffer_; }

    /// 内部使用
    void setCloseCallback(const CloseCallback &cb)
    { closeCallback_ = cb; }

    // called when TcpServer accepts a new connection
    void connectEstablished(); // should be called only once
    // called when TcpServer has removed me from its map
    void connectDestroyed(); // should be called only once

private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    void handleRead(TimeStamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();
    // void sendInLoop(string && message);
    void sendInloop(const StringPiece &message);
    void sendInloop(const void *data, size_t len);
    void shutdownInLoop();
    // void shutdownAndForceCloseInLoop(double seconds);
    void forceCloseInLoop();
    void setState(StateE s) { state_ = s; }
    const char* stateToString() const;
    void startReadInLoop();
    void stopReadInLoop();

    EventLoop                   *loop_;
    const string                name_;
    StateE                      state_;
    bool                        reading_;
    std::unique_ptr<Socket>     socket_;
    std::unique_ptr<Channel>    channel_;
    const InetAddress           localAddr_;
    const InetAddress           peerAddr_;
    ConnectionCallback          connectionCallback_;
    MessageCallback             messageCallback_;
    WriteCompleteCallback       writeCompleteCallback_;
    HighWaterMarkCallback       highWaterMarkCallback_;
    CloseCallback               closeCallback_;
    size_t                      highWaterMark_;
    Buffer                      inputBuffer_;
    Buffer                      outputBuffer_;  // FIXME use list<Buffer> as output buffer.
    boost::any                  context_;
    // FIXME creationTime_, lastReceiveTime_, bytesReceived_, bytesSent_
}; // class TcpConnection

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

} // namespace net

} // namespace sh

#endif // SH_NET_TCPCONNECTION_H
