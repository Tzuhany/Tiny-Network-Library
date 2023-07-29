#pragma once

#include "noncopyable.h"
#include "Timestamp.h"

#include <functional>
#include <memory>

class EventLoop;

/**
 * Channel：对文件描述符的封装，包含对事件的处理和回调
 */
class Channel : noncopyable {
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop *loop, int fd);

    ~Channel() = default;

    void handleEvent(Timestamp receiveTime);

    void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }

    void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }

    void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }

    void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

    void tie(const std::shared_ptr<void> &);

    int fd() const { return fd_; }

    int events() const { return events_; }

    void set_revents(uint32_t revt) { revents_ = revt; }

    void enableReading() {
        events_ |= kReadEvent;
        update();
    }

    void disableReading() {
        events_ &= ~kReadEvent;
        update();
    }

    void enableWriting() {
        events_ |= kWriteEvent;
        update();
    }

    void disableWriting() {
        events_ &= ~kWriteEvent;
        update();
    }

    void disableAll() {
        events_ = kNoneEvent;
        update();
    }

    bool isNoneEvent() const { return events_ == kNoneEvent; }

    bool isWriting() const { return events_ & kWriteEvent; }

    bool isReading() const { return events_ & kReadEvent; }

    int index() const { return index_; }

    void set_index(int idx) { index_ = idx; }

    EventLoop *ownerLoop() { return loop_; }

    // remove: 在loop中删除当前channel对象
    void remove();

private:

    // update: 通知loop更新当前channel
    void update();

    // handleEventWithGuard: 具体回调调用
    void handleEventWithGuard(Timestamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;
    const int fd_;
    // Channel 对象所关注的事件
    int events_;
    // 实际发生在fd上的事件
    uint32_t revents_;
    // 在当前loop中的状态，kAdd...
    int index_;

    // tied_ 用于管理 Channel 对象是否与其他对象建立了关联，确保在销毁 Channel 对象时，与之关联的其他对象仍然有效，避免悬挂指针问题
    std::weak_ptr<void> tie_;
    bool tied_;

    // 根据fd中发生的具体事件调用回调
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

