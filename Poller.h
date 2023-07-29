#pragma once

#include "noncopyable.h"
#include "Timestamp.h"

#include <vector>
#include <unordered_map>

class Channel;

/**
 * Poller: 抽象不同底层 I/O 多路复用技术
 */
class EventLoop;

class Poller : noncopyable {
public:
    using ChannelList = std::vector<Channel *>;

    explicit Poller(EventLoop *loop);

    virtual ~Poller() = default;

    virtual Timestamp poll(int timeoutMs, ChannelList *activeChannels) = 0;

    virtual void updateChannel(Channel *channel) = 0;

    virtual void removeChannel(Channel *channel) = 0;

    bool hasChannel(Channel *channel) const;

    static Poller *newDefaultPoller(EventLoop *loop);

protected:
    using ChannelMap = std::unordered_map<int, Channel *>;
    ChannelMap channels_;
private:
    EventLoop *ownerLoop_;
};