#include "Poller.h"
#include "EPollPoller.h"

#include <cstdlib>

Poller *Poller::newDefaultPoller(EventLoop *loop) {
    if (::getenv("NET_LIB_USE_POLL")) {
        return nullptr;
    } else {
        return new EPollPoller(loop);
    }
}