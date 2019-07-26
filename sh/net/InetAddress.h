#ifndef SH_NET_INETADDRESS_H
#define SH_NET_INETADDRESS_H

#include "sh/base/copyable.h"
#include "sh/base/StringPiece.h"

#include <netinet/in.h>

namespace sh
{

namespace net
{

namespace sockets
{
// 前向声明  SocketOps.h
const struct sockaddr* sockaddr_cast(const struct sockaddr_in6 *addr);

} // namespace sockets

/// 封装 sockaddr_in
class InetAddress : public copyable
{
public:
    /// 通过端口构造，经常用作监听
    explicit InetAddress(uint16_t port=0, bool loopbackOnly = false, bool ipv6 = false);
    /// 通过 ip 端口 构造
    /// @c ip should be "1.2.3.4"
    InetAddress(StringArg ip, uint16_t port, bool ipv6 = false);
    /// 通过 @c sockaddr_in 构造，经常用作接受新的连接
    explicit InetAddress(const struct sockaddr_in &addr)
        : addr_(addr)
    { }
    explicit InetAddress(const struct sockaddr_in6 &addr)
        : addr6_(addr)
    { }

    sa_family_t family() const { return addr_.sin_family; }
    string toIp() const;
    string toIpPort() const;
    uint16_t toPort() const;

    // 使用默认构造 赋值

    const struct sockaddr* getSockAddr() const { return sockets::sockaddr_cast(&addr6_); }
    void setSockAddrInet6(const struct sockaddr_in6 &addr6) { addr6_ = addr6; }

    uint32_t ipNetEndian() const;
    uint16_t portNetEndian() const { return addr_.sin_port; }

    // resolve hostname to IP address, 不改变 port 和 sin_family
    // 成功返回 true  , 线程安全
    static bool resolve(StringArg hostname, InetAddress *result);

    // set IPv6 ScopeID
    void setScopeId(uint32_t scope_id);

private:
    union
    {
       struct sockaddr_in   addr_;
       struct sockaddr_in6  addr6_;
    };
}; // class InetAddress

} // namespace net

} // namespace sh

#endif // SH_NET_INETADDRESS_H

// 在IPv6地址结构中（对应于IPv4的struct sockaddr_in），有一个我们非常陌生的字段scope_id
// 这个字段在我们使用链路本地地址来编程的时候是必须要使用的，这个字段表示我们需要选择接口ID。
// 为什么需要需要有这么一个字段，那是因为链路本地地址的特殊性，一个网络节点可以有多个网络接口，多个网络接口可以有相同的链路本地地址。
// 例如我们需要bind一个本地链路地址，这个时候就会有冲突，操作系统无法决策需要绑定的是哪个接口的本地链路地址。

// 又例如，如果我们在直连的2个主机之间直接用链路本地地址ping的话，会ping失败。

// 因此IPv6引入了scope_id来解决这个问题，scope_id指定了使用哪个网络接口。
