
#include "sh/base/Exception.h"
#include "sh/base/CurrentThread.h"

namespace sh
{

Exception::Exception(std::string msg)
    : message_(std::move(msg)), // c++11移动语义 也可以不用
      stack_(CurrentThread::stackTrace(/*demangle=*/false))
{
}

} // namespace sh

