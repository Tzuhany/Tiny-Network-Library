#include "EventLoop.h"
#include "Logger.h"
#include "Poller.h"
#include "Channel.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <cerrno>
#include <memory>

__thread EventLoop *t_loopInThisThread = nullptr;

const int kPollTimeMs = 10000;

int createEventfd() {
    int evtFd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtFd < 0) {
        LOG_FATAL("eventfd error:%d \n", errno);
    }
    return evtFd;
}

EventLoop::EventLoop()
        : looping_(false), quit_(false), callingPendingFunctors_(false), threadId_(CurrentThread::tid()),
          poller_(Poller::newDefaultPoller(this)), wakeupFd_(createEventfd()),
          wakeupChannel_(new Channel(this, wakeupFd_)) {
    LOG_DEBUG("EventLoop created %p in thread %d \n", this, threadId_);
    if (t_loopInThisThread) {
        LOG_FATAL("Another EventLoop %p exists in this thread %d \n", t_loopInThisThread, threadId_);
    } else {
        t_loopInThisThread = this;
    }

    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
    looping_ = true;
    quit_ = false;

    LOG_INFO("EventLoop %p start looping \n", this);

    while (!quit_) {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        for (Channel *channel: activeChannels_) {
            channel->handleEvent(pollReturnTime_);
        }
        doPendingFunctors();
    }

    LOG_INFO("EventLoop %p stop looping. \n", this);
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;

    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::runInLoop(const Functor &cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Functor &cb) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }

    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
}

void EventLoop::handleRead() const {
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR("EventLoop::handleRead() reads %lu bytes instead of 8", n);
    }
}

void EventLoop::wakeup() const {
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        LOG_ERROR("EventLoop::wakeup() writes %lu bytes instead of 8 \n", n);
    }
}

void EventLoop::updateChannel(Channel *channel) {
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel) {
    return poller_->hasChannel(channel);
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (const Functor &functor: functors) {
        functor();
    }

    callingPendingFunctors_ = false;
}