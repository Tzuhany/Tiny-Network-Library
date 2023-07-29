#pragma once

#include "Poller.h"
#include "Timestamp.h"

#include <vector>
#include <sys/epoll.h>

class Channel;

/**
 * EPollPoller: 使用 epoll 来实现底层的 I/O 多路复用
 */
class EPollPoller : public Poller {
public:
    explicit EPollPoller(EventLoop *loop);

    ~EPollPoller() override;

    // poll: 执行epoll_wait操作
    Timestamp poll(int timeoutMs, ChannelList *activeChannels) override;

    // updateChannel: channel对象更新操作
    void updateChannel(Channel *channel) override;

    // updateChannel: channel对象删除操作
    void removeChannel(Channel *channel) override;

private:
    static const int kInitEventListSize = 16;

    // fillActiveChannels: 将 EPoll 返回的就绪事件转换为对应的 Channel 对象，并将这些就绪的 Channel 对象存储在 activeChannels 容器中
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;

    // update: 负责epoll_ctl调用
    void update(int operation, Channel *channel) const;

    using EventList = std::vector<epoll_event>;

    int epollfd_;
    EventList events_;
};