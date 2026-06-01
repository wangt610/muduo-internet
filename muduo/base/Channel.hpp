#ifndef CHANNEL_HPP
#define CHANNEL_HPP
#include <functional>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
using socket_t = SOCKET;
#else
using socket_t = int;
#endif

// Channel：一个 fd 的事件管家（muduo 核心组件）
// 绑定一个 socket → 注册读/写/关闭/错误回调 → 通过 events() 告诉 Poller 监听什么 → 触发后分发回调
class Channel {
public:
    static const int kNoneEvent  = 0;
    static const int kReadEvent  = 1;
    static const int kWriteEvent = 2;
    static const int kErrorEvent = 4;
    static const int kCloseEvent = 8;

    using EventCallback = std::function<void()>;

    explicit Channel(socket_t fd);

    socket_t fd() const { return fd_; }

    // 供 Poller（epoll/select/WSAPoll）注册：当前关注的事件掩码
    int events() const { return events_; }

    // Poller::poll 返回后写入，再调用 handle_event()
    int revents() const { return revents_; }
    void set_revents(int rev) { revents_ = rev; }

    void enable_read();
    void disable_read();
    void enable_write();
    void disable_write();

    bool is_none_event() const { return events_ == kNoneEvent; }
    bool is_read_enabled() const  { return events_ & kReadEvent; }
    bool is_write_enabled() const { return events_ & kWriteEvent; }

    void set_read_callback(EventCallback cb)   { read_callback_ = std::move(cb); }
    void set_write_callback(EventCallback cb)  { write_callback_ = std::move(cb); }
    void set_close_callback(EventCallback cb)  { close_callback_ = std::move(cb); }
    void set_error_callback(EventCallback cb)  { error_callback_ = std::move(cb); }

    // 根据 revents_ 自动调用对应回调（由 EventLoop / Poller 在 poll 后触发）
    void handle_event();

private:
    socket_t fd_;
    int events_;   // 告诉 Poller 要监听什么
    int revents_;  // Poller 返回的已就绪事件

    EventCallback read_callback_;
    EventCallback write_callback_;
    EventCallback close_callback_;
    EventCallback error_callback_;
};

#endif // CHANNEL_HPP
