#ifndef SH_NET_ACCEPTOR_H
#define SH_NET_ACCEPTOR_H

#include "sh/net/Channel.h"
#include "sh/net/Socket.h"

#include <functional>

namespace sh
{

namespace net
{

class EventLoop;
class InetAddress;

/// tcp链接的接受者
class Acceptor : noncopyable
{
public:
    typedef std::function<void (int sockfd, const InetAddress &)> NewConnectionCallback;

    Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback &cb)
    { newConnectionCallback_ = cb; }

    bool listenning() const { return listenning_; }
    void listen();

private:
    void handleRead();

    EventLoop               *loop_;
    Socket                  acceptSocket_;
    Channel                 acceptChannel_;
    NewConnectionCallback   newConnectionCallback_;
    bool                    listenning_;
    int                     idleFd_; //  idle 闲置的 闲散的
}; // class Acceptor

} // namespace net

} // namespace sh

#endif // SH_NET_ACCEPTOR_H
