
#include "sh/net/Poller.h"
#include "sh/net/PollPoller.h"
#include "sh/net/EPollPoller.h"

#include <stdlib.h>

using namespace sh::net;

// 默认EPoll
Poller* Poller::newDefaultPoller(EventLoop* loop)
{
  if (::getenv("SH_USE_POLL"))
  {
    return new PollPoller(loop);
  }
  else
  {
    return new EPollPoller(loop);
  }
}
