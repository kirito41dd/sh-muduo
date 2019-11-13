
#include "sh/net/http/HttpServer.h"
#include "sh/net/http/HttpRequest.h"
#include "sh/net/http/HttpResponse.h"
#include "sh/net/EventLoop.h"
#include "sh/base/Logging.h"

#include <iostream>
#include <map>

using namespace sh;
using namespace sh::net;
using namespace sh::net::http;
using namespace std;

void onRequest(const HttpRequest &req, HttpResponse *resp)
{
    cout << "headers " << req.methodString() << " path " << req.path() << " query " << req.query() << endl;
    const map<string,string> &headers = req.headers();
    for (const auto& header : headers)
    {
        cout << header.first << ": " << header.second << endl;
    }
    if (req.path() == "/")
    {
        resp->setStatusCode(HttpResponse::k200Ok);
        resp->setStatusMessage("OK");
        resp->setContentType("text/html");
        resp->addHeader("Server", "sh-muduo");
        string now = TimeStamp::localNow().toFormattedString(0);
        resp->setBody("<html><head><title>hello sh-muduo</title></head>"
                      "<body><h1>hellow</h1>Now is " + now + "</body></html>");
    }
    else if (0)
    {

    }
}

int main()
{
    Logger::setLogLevel(Logger::DEBUG);
    int numThreads = 0;
    EventLoop loop;
    HttpServer server(&loop, InetAddress(9999), "test-http");
    server.setHttpCallback(onRequest);
    server.setThreadNum(numThreads);
    server.start();
    loop.loop();
    return 0;
}
