#ifndef SH_NET_HTTP_HTTPRESPONSE_H
#define SH_NET_HTTP_HTTPRESPONSE_H

#include "sh/base/copyable.h"
#include "sh/base/Types.h"

#include <map>

namespace sh
{
namespace net
{
class Buffer; // 前向声明 forward declaration
namespace http
{

class HttpResponse : public copyable
{
public:
    enum HttpStatusCode
    {
        kUnknown,
        k200Ok = 200,
        k301MovedPermanently = 301, // 永久重定向 Permanently [ˈpɜːmənəntli]  永久地; 永远; 长期; 一直;
        k400BadRequest = 400,
        k404NotFound = 404,
    };

    explicit HttpResponse(bool close)
        : statusCode_(kUnknown),
          closeConnection_(close)
    {
    }
    ~HttpResponse() = default;

    void setStatusCode(HttpStatusCode code) { statusCode_ = code; }
    void setStatusMessage(const string &message) { statusMessage_ = message; }
    void setCloseConnection(bool on) { closeConnection_ = on; }

    bool closeConnection() const { return closeConnection_; }

    void addHeader(const string &key, const string &value) { headers_[key] = value; }

    void setContentType(const string &contentType) { addHeader("Content-Type", contentType); }
    void setBody(const string &body) { body_ = body; }

    void appendToBuffer(Buffer *output) const;

private:
    std::map<string,string> headers_;
    HttpStatusCode          statusCode_;
    // FIXME: add http version
    string                  statusMessage_;
    bool                    closeConnection_;
    string                  body_;
}; // class HttpResponse

} // namespace http
} // namespace net
} // namespace sh

#endif // SH_NET_HTTP_HTTPRESPONSE_H
