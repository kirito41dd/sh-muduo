#ifndef SH_NET_SOCKET_H
#define SH_NET_SOCKET_H

#include "sh/base/noncopyable.h"

// 前向声明 <netinet/tcp.h>
struct tcp_info;

namespace sh
{

namespace net
{

class InetAddress; // forward declaration

/// 封装 socket 文件描述符
/// 在析构的时候自动关闭 线程安全,所有操作都交给OS
class Socket : noncopyable
{
public:
    explicit Socket(int sockfd)
        :sockfd_(sockfd)
    { }
    // Socket(Socket &&) c++11
    ~Socket();

    int fd() const { return sockfd_; }
    // return true if success.
    bool getTcpInfo(struct tcp_info *tcpi) const;
    bool getTcpInfoString(char *buf, int len) const;

    /// abort if address in use
    void bindAddress(const InetAddress &localaddr);
    /// abort if address in use
    void listen();

    /// 成功返回非负整数描述符 , 出错返回-1
    /// 设置non-blocking, 对端地址返回给 *peeraddr
    int accept(InetAddress *peeraddr);

    void shutdownWrite();

    /// Enable/disable TCP_NODELAY ，延时确认机制
    void setTcpNoDelay(bool on);

    /// Enable/disable SO_REUSEADDR, 地址复用
    void setReuseAddr(bool on);

    /// Enable/disable SO_REUSEPORT,端口复用
    void setReusePort(bool on);

    /// Enable/disable SO_KEEPALIVE, 保活探测
    void setKeepAlive(bool on);

private:
    const int sockfd_;
}; // class Socket

} // namespace sh

} // namespace sh

#endif // SH_NET_SOCKET_H
