#ifndef SH_BASE_LOGSTREAM_H
#define SH_BASE_LOGSTREAM_H

#include "sh/base/noncopyable.h"
#include "sh/base/StringPiece.h"
#include "sh/base/Types.h"

#include <assert.h>
#include <string.h>  // for memcpy

namespace sh
{

namespace detail
{

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

// 非类型参数的模板类 目前C++的标准仅支持整型、枚举、指针类型和引用类型
template<int SIZE>
class FixedBuffer : noncopyable
{
public:
    FixedBuffer()
        : cur_(data_)
    {
        setCookie(cookieStart);
    }

    ~FixedBuffer()
    {
        setCookie(cookieEnd);
    }

    void append(const char* buf, size_t len)
    {
        if (implicit_cast<size_t>(avail()) > len)
        {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    const char* data() const { return data_; }
    int length() const { return static_cast<int>(cur_ - data_); }

    // 直接写入data_
    char* current() { return cur_; }
    int avail() const { return static_cast<int>(end() - cur_); }
    void add(size_t len) { cur_ += len; }

    void reset() {cur_ = data_; }
    void bzero() { memZero(data_, sizeof data_); }

    // for used by GDB
    const char* debugString();
    void setCookie(void (*cookie)()) { cookie_ = cookie; }
    // for used by unit test
    string toString() const { return string(data_, length()); }
    StringPiece toStringPiece() const { return StringPiece(data_, length()); }

private:
    const char* end() const { return data_ + sizeof data_; }  // 数据末端

    static void cookieStart();
    static void cookieEnd();

    void (*cookie_)();
    char data_[SIZE]; // [ data, end() )  区间左闭右开
    char *cur_; // 指向下一个空位
};

} // namesapce detail

class  LogStream : noncopyable
{
public:
    typedef LogStream self;
    typedef detail::FixedBuffer<detail::kSmallBuffer> Buffer;

    self& operator<<(bool v)
    {
        buffer_.append(v ? "1" : "0", 1);
        return *this;
    }
    self& operator<<(short v);
    self& operator<<(unsigned short v);
    self& operator<<(int v);
    self& operator<<(unsigned int v);
    self& operator<<(long v);
    self& operator<<(unsigned long v);
    self& operator<<(long long v);
    self& operator<<(unsigned long long v);
    self& operator<<(const void* v);
    // self& operator<<(long double);
    self& operator<<(double);
    self& operator<<(float v)
    {
        return *this << static_cast<double>(v);
    }
    self& operator<<(char v)
    {
        buffer_.append(&v,1);
        return *this;
    }
    // self& operator<<(signed char);
    // self& operator<<(unsigned char);
    self& operator<<(const char *str)
    {
        if (str) {
            buffer_.append(str, strlen(str));
        } else {
            buffer_.append("(null)", 6);
        }
        return *this;
    }
    self& operator<<(const unsigned char *str)
    {
        return operator<<(reinterpret_cast<const char *>(str));
    }
    self& operator<<(const string &v)
    {
        buffer_.append(v.c_str(), v.size());
        return *this;
    }
    self& operator<<(const StringPiece &v)
    {
        buffer_.append(v.data(),v.size());
        return *this;
    }
    self& operator<<(const Buffer &v)
    {
        *this << v.toStringPiece();
        return *this;
    }

    void append(const char *data, int len) { buffer_.append(data, len); }
    const Buffer& buffer() const { return buffer_; }
    void resetBuffer() { buffer_.reset(); }

private:
    void staticCheck();

    template<typename T>
    void formatInteger(T);

    Buffer buffer_;

    static const int kMaxNumericSize = 32;
}; // class LogStream

class Fmt //: noncopyable
{
public:
    template<typename T>
    Fmt(const char *fmt, T val);

    const char* data() const { return buf_; }
    int length() const { return length_; }

private:
    char    buf_[32];
    int     length_;
}; // class Fmt

inline LogStream& operator<<(LogStream &s, const Fmt &fmt)
{
    s.append(fmt.data(), fmt.length());
    return s;
}

// Format quantity n in SI units (k, M, G, T, P, E).
// 返回的字符串最大长度5.
// Requires n >= 0
string formatSI(int64_t n);

// Format quantity n in IEC (binary) units (Ki, Mi, Gi, Ti, Pi, Ei).
// 返回的字符串最大长度6.
// Requires n >= 0
string formatIEC(int64_t n);

} // namespace sh

#endif // SH_BASE_LOGSTREAM_H
