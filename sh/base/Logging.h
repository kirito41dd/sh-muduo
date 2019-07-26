#ifndef SH_BASE_LOGGING_H
#define SH_BASE_LOGGING_H

#include "sh/base/LogStream.h"
#include "sh/base/TimeStamp.h"

namespace sh
{

class Logger
{
public:
    enum LogLevel{
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS
    };

    // 编译阶段计算源文件 basename
    class SourceFile
    {
    public:
        template<int N>
        SourceFile(const char (&arr)[N])
            : data_(arr),
              size_(N-1)
        {
            // char *strrchr(const char *str, int c)
            // 在参数 str 所指向的字符串中搜索最后一次出现字符 c（一个无符号字符）的位置。
            const char *slash = strrchr(data_, '/');
            if (slash)
            {
                data_ = slash + 1;
                size_ -= static_cast<int>(data_ - arr);
            }
        }

        explicit SourceFile(const char *filename)
            : data_(filename)
        {
            const char *slash = strrchr(data_, '/');
            if (slash)
            {
                data_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(data_));
        }

        const char *data_;
        int        size_;
    }; // class SourceFile

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream& stream() { return impl_.stream_; }

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    typedef void (*OutputFunc)(const char *msg, int len);
    typedef void (*FlushFunc)();
    static void setOutput(OutputFunc func);
    static void setFlush(FlushFunc func);
    //static void setTimeZone(const TimeZone& tz)


private:
    class Impl
    {
    public:
        typedef Logger::LogLevel LogLevel;
        Impl(LogLevel level, int old_errno, const SourceFile &file, int line);
        void formatTime();
        void finish();

        TimeStamp   time_;
        LogStream   stream_;
        LogLevel    level_;
        int         line_;
        SourceFile  basename_;
    }; // class Impl

    Impl impl_;
}; // class Logger

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel()
{
    return g_logLevel;
}

// if (good)
//   LOG_INFO << "Good news";
// else
//   LOG_WARN << "Bad news";
#define LOG_TRACE if (sh::Logger::logLevel() <= sh::Logger::TRACE) \
  sh::Logger(__FILE__, __LINE__, sh::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (sh::Logger::logLevel() <= sh::Logger::DEBUG) \
  sh::Logger(__FILE__, __LINE__, sh::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (sh::Logger::logLevel() <= sh::Logger::INFO) \
  sh::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN sh::Logger(__FILE__, __LINE__, sh::Logger::WARN).stream()
#define LOG_ERROR sh::Logger(__FILE__, __LINE__, sh::Logger::ERROR).stream()
#define LOG_FATAL sh::Logger(__FILE__, __LINE__, sh::Logger::FATAL).stream()
#define LOG_SYSERR sh::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL sh::Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int savedErrno);

// 检查是否不为null，是null输出日志
#define CHECK_NOTNULL(val) \
    ::sh::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))
template<typename T>
T* CheckNotNull(Logger::SourceFile file, int line, const char *names, T *ptr)
{
    if (ptr == NULL)
    {
        Logger(file, line, Logger::FATAL).stream() << names;
    }
    return ptr;
}

} // namespace sh

#endif // SH_BASE_LOGGING_H
