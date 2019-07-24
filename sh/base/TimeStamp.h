#ifndef SH_BASE_TIMESTAMP_H
#define SH_BASE_TIMESTAMP_H

#include "sh/base/Types.h"
#include "sh/base/copyable.h"

namespace sh
{

// 微秒级时间戳  UTC 0区 世界时间 支持获取本地时间
class TimeStamp : public sh::copyable
                  //public boost::equality_comparable<TimeStamp>, // 去掉boost，自己实现
                  //public boost::less_than_comparable<TimeStamp>
{
public:
    /// Constucts an invalid Timestamp.
    TimeStamp()
        : microSecondsSinceEpoch_(0)
    {
    }

    /// Constucts a Timestamp at specific time
    /// @param microSecondsSinceEpoch
    explicit TimeStamp(int64_t microSecondsSinceEpochArg)
        : microSecondsSinceEpoch_(microSecondsSinceEpochArg)
    {
    }

    void swap(TimeStamp &other)
    {
        std::swap(microSecondsSinceEpoch_, other.microSecondsSinceEpoch_);
    }

    // 使用默认的 拷贝/赋值/析构 即可

    string toString() const;
    string toFormattedString(bool showMicroseconds = true) const;

    // 是否有效
    bool valid() const { return microSecondsSinceEpoch_ > 0; }

    // 获取从1970.1.1过去了多少时间
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
    time_t secondsSinceEpoch() const
    { return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondPerSecond); }

    /// get UTC+0 time of now.
    static TimeStamp now();
    /// get local time of now.
    static TimeStamp localNow();
    static TimeStamp invalid() { return TimeStamp(); } // 获取一个无效时间

    static TimeStamp fromUnixTime(time_t t)
    {
        return fromUnixTime(t,0);
    }

    static TimeStamp fromUnixTime(time_t t, int microseconds)
    {
        return TimeStamp(static_cast<int64_t>(t) * kMicroSecondPerSecond + microseconds);
    }

    static const int kMicroSecondPerSecond = 1000 * 1000; //一秒等于多少微秒
private:
    int64_t microSecondsSinceEpoch_;
}; // class TimeStamp

// 比较操作
inline bool operator<(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}
inline bool operator==(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

// 如果使用boost的less_than_comparable 和 equality_comparable ，则只提供上面两个比较就可以
// 不使用需要自己实现
inline bool operator>(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.microSecondsSinceEpoch() > rhs.microSecondsSinceEpoch();
}
inline bool operator<=(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch()
        || lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}
inline bool operator>=(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.microSecondsSinceEpoch() > rhs.microSecondsSinceEpoch()
        || lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline bool operator!=(TimeStamp lhs, TimeStamp rhs)
{
    return ! (lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch());
}

/// get time difference of two timestamps
/// @param high,low
/// @return high-low in seconds
inline double timeDifference(TimeStamp high, TimeStamp low)
{
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / TimeStamp::kMicroSecondPerSecond;
}

/// add seconds to given timestamp
/// @return timestamp + seconds as new TimeStamp
inline TimeStamp addTime(TimeStamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * TimeStamp::kMicroSecondPerSecond);
    return TimeStamp(timestamp.microSecondsSinceEpoch() + delta);
}

} // namespace sh

#endif // SH_BASE_TIMESTAMP_H
