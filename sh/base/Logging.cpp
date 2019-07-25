
#include "sh/base/Logging.h"

#include "sh/base/CurrentThread.h"
#include "sh/base/TimeStamp.h"
//#include "sh/base/TimeZone.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sstream>

namespace sh
{

__thread char       t_errnobuf[512];
__thread char       t_time[64];
__thread time_t     t_lastSecond;

const char* strerror_tl(int savedErrno)
{
    return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}

Logger::LogLevel initLogLevel()
{
    if (::getenv("SH_LOG_TRACE"))
        return Logger::TRACE;
    else if (::getenv("SH_LOG_DEBUG"))
        return Logger::DEBUG;
    else
        return Logger::INFO;
}

Logger::LogLevel g_logLevel = initLogLevel();

const char* LogLevelName[Logger::NUM_LOG_LEVELS] =
{
    "TRACE ",  // len = 6 bytes
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL "
};

// 编译期获取字符串长度
class T
{
public:
    T(const char *str, unsigned len)
        : str_(str),
          len_(len)
    {
        assert(strlen(str) == len_);
    }
    const char      *str_;
    const unsigned  len_;
};

inline LogStream& operator<<(LogStream &s, T v)
{
    s.append(v.str_, v.len_);
    return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v)
{
  s.append(v.data_, v.size_);
  return s;
}

void defaultOutput(const char *msg, int len)
{
    size_t n = fwrite(msg, 1, len, stdout);
    // check n...
    (void) n;
}

void defaultFlush()
{
    fflush(stdout);
}

Logger::OutputFunc g_output = defaultOutput;
Logger::FlushFunc  g_flush = defaultFlush;

//*///////////////////////////////////////////////////////////////////////////////////////////////////////

Logger::Impl::Impl(Logger::Impl::LogLevel level, int old_errno, const Logger::SourceFile &file, int line)
    : time_(TimeStamp::localNow()), // 原来是now，我不想用timezone，直接用本地时间
      stream_(),
      level_(level),
      line_(line),
      basename_(file)
{
    formatTime();
    CurrentThread::tid();
    stream_ << T(CurrentThread::tidString(), CurrentThread::tidStringLength());
    stream_ << T(LogLevelName[level], 6);
    if(old_errno != 0)
    {
        stream_ << strerror_tl(old_errno) << " (errno =" << old_errno << ") ";
    }
}

void Logger::Impl::formatTime()
{
    int64_t microSecondSinceEpoch = time_.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microSecondSinceEpoch / TimeStamp::kMicroSecondPerSecond);
    int microseconds = static_cast<time_t>(microSecondSinceEpoch % TimeStamp::kMicroSecondPerSecond);
    if (seconds != t_lastSecond)
    {
        t_lastSecond = seconds;
        struct tm tm_time;
        ::gmtime_r(&seconds, &tm_time);

        int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d", /* len = 17 */
                           tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                           tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        assert(len == 17);
    }
    Fmt us(".%06dZ ", microseconds);
    assert(us.length() == 9);
    stream_ << T(t_time, 17) << T(us.data(), 9);
}

void Logger::Impl::finish()
{
    stream_ << " - " << basename_ << ":" << line_ << "\n";
}

Logger::Logger(Logger::SourceFile file, int line)
    : impl_(INFO, 0, file, line)
{
}

Logger::Logger(Logger::SourceFile file, int line, Logger::LogLevel level)
    : impl_(level, 0, file, line)
{
}

Logger::Logger(Logger::SourceFile file, int line, Logger::LogLevel level, const char *func)
    : impl_(level, 0, file, line)
{
    impl_.stream_ << func << " ";
}

Logger::Logger(Logger::SourceFile file, int line, bool toAbort)
    : impl_(toAbort ? FATAL : ERROR, errno, file, line)
{
}

Logger::~Logger()
{
    impl_.finish();
    const LogStream::Buffer& buf(stream().buffer());
    g_output(buf.data(), buf.length());
    if(impl_.level_ == FATAL)
    {
        g_flush();
        abort();
    }
}



} // namespace sh
