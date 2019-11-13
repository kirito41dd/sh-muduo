#ifndef SH_NET_HTTP_HTTPCONTEXT_H
#define SH_NET_HTTP_HTTPCONTEXT_H

#include "sh/base/copyable.h"
#include "sh/net/http/HttpRequest.h"

namespace sh
{
namespace net
{
class Buffer;
namespace http
{

class HttpContext : public copyable
{
public:
    enum HttpRequestParseState
    {
        kExpectRequestLine, // expect v. 预料; 预期; 预计; 等待; 期待; 盼望; 要求; 指望;
        kExpectHeaders,
        kExpectBody,
        kGotAll,
    };

    HttpContext()
        : state_(kExpectRequestLine)
    {
    }
    ~HttpContext() = default;

    // return false if any error
    bool parseRequest(Buffer *buf, TimeStamp receiveTime);

    bool gotAll() const { return state_ == kGotAll; }

    void reset()
    {
        state_ = kExpectRequestLine;
        HttpRequest dummy; // [ˈdʌmi] 人体模型; 仿制品; 仿造物; 笨蛋; 蠢货;
        request_.swap(dummy);
    }

    const HttpRequest& request() const { return request_; }
    HttpRequest& request() { return request_; }

private:
    bool processRequestLine(const char *begin, const char *end);

    HttpRequestParseState   state_;
    HttpRequest             request_;
}; // class HttpContext

} // namespace http
} // namespace net
} // namespace sh

#endif // SH_NET_HTTP_HTTPCONTEXT_H
