#ifndef SH_BASE_DATE_H
#define SH_BASE_DATE_H

#include "sh/base/copyable.h"
#include "sh/base/Types.h"

struct tm; // 前向声明 在time.h中

namespace sh
{

class Date : public sh::copyable
{
public:

    struct YearMonthDay
    {
        int year;   // [1900-2500]
        int month;  // [1-12]
        int day;    // [1-31]
    };

    static const int kDaysPerWeek = 7;
    static const int kJulianDayOf1970_01_01; // 这个在cpp初始化

    /// 构造一个无效Date
    Date()
        : julianDayNumber_(0)
    {}

    /// 构造一个日期 yyyy-mm-dd
    /// @param year month day
    Date(int year, int month, int day);

    /// 构造一个日期 距离 julain 的天数
    explicit Date(int julianDayNum)
        : julianDayNumber_(julianDayNum)
    {}

    /// 构造一个日期 从 tm 结构体
    explicit Date(const struct tm &t);

    // 获取一个本地日期
    static Date getLocalDate();

    // 使用默认拷贝和赋值

    void swap(Date & other)
    {
        std::swap(julianDayNumber_, other.julianDayNumber_);
    }

    // 是否有效日期
    bool valid() const { return julianDayNumber_ > 0; }

    /// 转化成 yyyy-mm-dd 字符串
    string toIsoString() const;

    /// 返回一个YearMonthDay结构体
    struct YearMonthDay yearMonthDay() const;

    int year()  const { return yearMonthDay().year; }
    int month() const { return yearMonthDay().month; }
    int day()   const { return yearMonthDay().day; }

    // [0 1 ... 6] => [sunday monday ... saturday]
    int weekDay() const { return (julianDayNumber_+1) % kDaysPerWeek; }

    int julianDayNumber() const { return julianDayNumber_; }

private:
    int julianDayNumber_; // 儒略日数（Julian Day Number，JDN）的计算是从格林威治标准时间的中午开始
}; // class Date

// operator < <= > >= == !=
inline bool operator<(Date x, Date y)
{
    return x.julianDayNumber() < y.julianDayNumber();
}
inline bool operator>(Date x, Date y)
{
    return x.julianDayNumber() > y.julianDayNumber();
}
inline bool operator<=(Date x, Date y)
{
    return x.julianDayNumber() <= y.julianDayNumber();
}
inline bool operator>=(Date x, Date y)
{
    return x.julianDayNumber() >= y.julianDayNumber();
}

inline bool operator==(Date x, Date y)
{
    return x.julianDayNumber() == y.julianDayNumber();
}
inline bool operator!=(Date x, Date y)
{
    return x.julianDayNumber() != y.julianDayNumber();
}

} // namespace sh

#endif // SH_BASE_DATE_H
