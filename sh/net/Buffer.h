#ifndef SH_NET_BUFFER_H
#define SH_NET_BUFFER_H

#include "sh/base/copyable.h"
#include "sh/base/StringPiece.h"
#include "sh/base/Types.h"

#include "sh/net/Endian.h"

#include <algorithm>
#include <vector>

#include <assert.h>
#include <string.h>

namespace sh
{

namespace net
{

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode
class Buffer : copyable
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
        : buffer_(kCheapPrepend + initialSize),
          readerIndex_(kCheapPrepend),
          writerInedx_(kCheapPrepend)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == initialSize);
        assert(prependableBytes() == kCheapPrepend);
    }

    // 使用内置 拷贝构造 移动构造(g++4.6) 赋值 析构

    void swap(Buffer &rhs)
    {
        buffer_.swap(rhs.buffer_);
        std::swap(readerIndex_, rhs.readerIndex_);
        std::swap(writerInedx_, rhs.writerInedx_);
    }

    size_t readableBytes() const
    { return writerInedx_ - readerIndex_; }

    size_t writableBytes() const
    { return buffer_.size() - writerInedx_; }

    size_t prependableBytes() const
    { return readerIndex_; }

    const char* peek() const
    { return begin() + readerIndex_;}

    const char* findCRLF() const
    {
        const char *crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2); // 查找子序列
        return crlf == beginWrite() ? NULL : crlf;
    }

    const char* findCRLF(const char *start) const
    {
        assert(peek() <= start);
        assert(start <= beginWrite());
        const char *crlf = std::search(start, beginWrite(), kCRLF, kCRLF+2); // 查找子序列
        return crlf == beginWrite() ? NULL : crlf;
    }

    const char* findEOL() const
    {
        const void* eol = memchr(peek(), '\n', readableBytes()); // 搜索字符
        return static_cast<const char*>(eol);
    }

    const char* findEOL(const char *start) const
    {
        assert(peek() <= start);
        assert(start <= beginWrite());
        const void* eol = memchr(start, '\n', beginWrite() - start); // 搜索字符
        return static_cast<const char*>(eol);
    }

    // 数据已取出，移动指针
    void retrieve(size_t len)
    {
        assert(len <= readableBytes());
        if (len < readableBytes())
        {
            readerIndex_ += len;
        }
        else
        {
            retrieveAll();
        }
    }

    void retrieveUntil(const char *end)
    {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    void retrieveInt64()
    {
        retrieve(sizeof(int64_t));
    }

    void retrieveInt32()
    {
        retrieve(sizeof(int32_t));
    }

    void retrieveInt16()
    {
        retrieve(sizeof(int16_t));
    }

    void retrieveInt8()
    {
        retrieve(sizeof(int8_t));
    }

    void retrieveAll()
    {
        readerIndex_ = kCheapPrepend;
        writerInedx_ = kCheapPrepend;
    }

    string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    string retrieveAsString(size_t len)
    {
        assert(len <= readableBytes());
        string result(peek(), len);
        retrieve(len);
        return result;
    }

    StringPiece toStringPiece() const
    {
        return StringPiece(peek(), static_cast<int>(readableBytes()));
    }

    void append(const StringPiece &str)
    {
        append(str.data(), str.size());
    }

    void append(const char *data, size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data, data+len, beginWrite());
        hasWritten(len);
    }

    void append(const void *data, size_t len)
    {
        append(static_cast<const char*>(data), len);
    }

    void ensureWritableBytes(size_t len)
    {
        if (writableBytes() < len)
        {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    char* beginWrite()
    { return begin() + writerInedx_; }

    const char* beginWrite() const
    { return begin() + writerInedx_; }

    void hasWritten(size_t len)
    {
        assert(len <= writableBytes());
        writerInedx_ += len;
    }

    void unwrite(size_t len)
    {
        assert(len <= readableBytes());
        writerInedx_ -= len;
    }

    /// 以网络字节序写入int64_t
    void appendInt64(int64_t x)
    {
        int64_t be64 = sockets::hostToNetwork64(x);
        append(&be64, sizeof be64);
    }

    /// 以网络字节序写入int32_t
    void appendInt32(int32_t x)
    {
        int32_t be32 = sockets::hostToNetwork32(x);
        append(&be32, sizeof be32);
    }

    /// 以网络字节序写入int16_t
    void appendInt16(int16_t x)
    {
        int16_t be16 = sockets::hostToNetwork16(x);
        append(&be16, sizeof be16);
    }

    /// 以网络字节序写入int8_t
    void appendInt8(int8_t x)
    {
        append(&x, sizeof x);
    }

    /// 读出一个 int64_t 网络字节序自动转换为主机字节序
    int64_t readInt64()
    {
        int64_t result = peekInt64();
        retrieveInt64();
        return result;
    }

    /// 读出一个 int32_t 网络字节序自动转换为主机字节序
    int32_t readInt32()
    {
        int32_t result = peekInt32();
        retrieveInt32();
        return result;
    }

    /// 读出一个 int16_t 网络字节序自动转换为主机字节序
    int16_t readInt16()
    {
        int16_t result = peekInt16();
        retrieveInt16();
        return result;
    }

    /// 读出一个 int8_t 网络字节序自动转换为主机字节序
    int8_t readInt8()
    {
        int8_t result = peekInt8();
        retrieveInt8();
        return result;
    }


    /// peek int64_t 网络字节序自动转换为主机字节序
    int64_t peekInt64() const
    {
        assert(readableBytes() >= sizeof(int64_t));
        int64_t be64 = 0;
        ::memcpy(&be64, peek(), sizeof(be64));
        return sockets::networkToHost64(be64);
    }

    /// peek int32_t 网络字节序自动转换为主机字节序
    int32_t peekInt32() const
    {
        assert(readableBytes() >= sizeof(int32_t));
        int32_t be32 = 0;
        ::memcpy(&be32, peek(), sizeof(be32));
        return sockets::networkToHost32(be32);
    }

    /// peek int16_t 网络字节序自动转换为主机字节序
    int16_t peekInt16() const
    {
        assert(readableBytes() >= sizeof(int16_t));
        int16_t be16 = 0;
        ::memcpy(&be16, peek(), sizeof(be16));
        return sockets::networkToHost16(be16);
    }

    /// peek int8_t
    int8_t peekInt8() const
    {
        assert(readableBytes() >= sizeof(int8_t));
        int8_t x = *peek();
        return x;
    }


    /// prepend int64_t 以网络字节序
    void prependInt64(int64_t x)
    {
        int64_t netint = sockets::hostToNetwork64(x);
        prepend(&netint, sizeof netint);
    }

    /// prepend int32_t 以网络字节序
    void prependInt32(int32_t x)
    {
        int32_t netint = sockets::hostToNetwork32(x);
        prepend(&netint, sizeof netint);
    }

    /// prepend int16_t 以网络字节序
    void prependInt16(int16_t x)
    {
        int16_t netint = sockets::hostToNetwork16(x);
        prepend(&netint, sizeof netint);
    }

    /// prepend int8_t 以网络字节序
    void prependInt8(int8_t x)
    {
        prepend(&x, sizeof x);
    }

    void prepend(const void *data, size_t len)
    {
        assert(len <= prependableBytes());
        readerIndex_ -= len;
        const char *d = static_cast<const char*>(data);
        std::copy(d, d+len, begin() + readerIndex_);
    }

    /// 缩小 保留 @c reserver 大小的空间
    void shrink(size_t reserver)
    {
        // FIXME: use vector::shrink_to_fit() in C++ 11 if possible.
        Buffer other;
        other.ensureWritableBytes(readableBytes() + reserver);
        other.append(toStringPiece());
        swap(other);
    }

    size_t internalCapacity() const
    {
        return buffer_.capacity();
    }

    /// Read data directly into buffer.
    ///
    /// It may implement with readv(2)
    /// @return result of read(2), @c errno is saved
    ssize_t readFd(int fd, int* savedErrno);

private:

    char* begin()
    { return &*buffer_.begin(); }

    const char* begin() const
    { return &*buffer_.begin(); }

    void makeSpace(size_t len)
    {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            // FIXME move readable data
            buffer_.resize(writerInedx_ + len);
        }
        else
        {
            // move readable data to the front, make space inside buffer
            assert(kCheapPrepend < readerIndex_);
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_,
                      begin() + writerInedx_,
                      begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerInedx_ = readerIndex_ + readable;
            assert(readable == readableBytes());
        }
    }

private:
    std::vector<char>   buffer_;
    size_t              readerIndex_;
    size_t              writerInedx_;

    static const char kCRLF[];
}; // class Buffer

} // namespace net

} // namespace sh

#endif // SH_NET_BUFFER_H
