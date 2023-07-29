#pragma once

#include <functional>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

#include "noncopyable.h"
#include "Timestamp.h"
#include "CurrentThread.h"

class Channel;

class Poller;

class EventLoop : noncopyable {
public:
    using Functor = std::function<void()>;

    EventLoop();

    ~EventLoop();

    void loop();

    void quit();

    Timestamp pollReturnTime() const { return pollReturnTime_; }

    void runInLoop(const Functor& cb);

    void queueInLoop(const Functor& cb);

    void wakeup() const;

    void updateChannel(Channel *channel);

    void removeChannel(Channel *channel);

    bool hasChannel(Channel *channel);

    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

private:
    void handleRead() const;

    void doPendingFunctors();

    using ChannelList = std::vector<Channel *>;

    std::atomic_bool looping_;
    std::atomic_bool quit_;

    const pid_t threadId_;

    Timestamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;

    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;

    std::atomic_bool callingPendingFunctors_;
    std::vector<Functor> pendingFunctors_;
    std::mutex mutex_;
};