
#include "sh/base/Thread.h"
#include "sh/base/CurrentThread.h"

#include <unistd.h>  // syscall
#include <errno.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/syscall.h> // SYS_gettid
#include <sys/types.h>
#include <linux/unistd.h>

namespace sh
{
namespace detail
{

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

} // namespace detail

void CurrentThread::cacheTid()
{
    if (t_cachedTid == 0)
    {
        t_cachedTid = detail::gettid();
        t_tidStringLenght = snprintf(t_tidString, sizeof t_tidString, "%5d", t_cachedTid);
    }
}

} // namespace sh
