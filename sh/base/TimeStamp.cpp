
#include "sh/base/TimeStamp.h"

#include <sys/time.h>
#include <stdio.h>

// int64_t用来表示64位整数，在32位系统中是long long int，在64位系统中是long int，所以打印int64_t的格式化方法是：
// printf("%ld", value); // 64bit OS
// printf("%lld", value); // 32bit OS
// 为了统一，使用下面宏，printf("%" PRId64 "\n", value); 可以使用 PRId64
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>  // PRId64

using namespace sh;

// 静态断言，编译期，条件为false打印提示
static_assert(sizeof(TimeStamp) == sizeof(int64_t),
              "TimeStamp is should be int64_t"); // https://github.com/chenshuo/muduo/issues/399





sh::string TimeStamp::toString() const
{
    char buf[32] = {0};
    int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondPerSecond;
    int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondPerSecond;
    snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
    return buf; //char * 会构造成 string
}

sh::string TimeStamp::toFormattedString(bool showMicroseconds) const
{
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);

    if (showMicroseconds)
      {
        int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondPerSecond);
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                 microseconds);
      }
      else
      {
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
      }
    return buf;
}

// static
TimeStamp TimeStamp::now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return TimeStamp(seconds * kMicroSecondPerSecond + tv.tv_usec);
}

TimeStamp TimeStamp::localNow()
{
    struct timeval tv;
    struct tm t;
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &t);  // 本地时区   如北京 utc+8
    int64_t seconds = tv.tv_sec + t.tm_gmtoff /*时区偏移*/;
    return TimeStamp(seconds * kMicroSecondPerSecond + tv.tv_usec);
}










