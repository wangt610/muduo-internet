#include "Channel.hpp"

Channel::Channel(socket_t fd)
    : fd_(fd),
      events_(kNoneEvent),
      revents_(kNoneEvent)
{}

void Channel::enable_read()
{
    events_ |= kReadEvent;
}

void Channel::disable_read()
{
    events_ &= ~kReadEvent;
}

void Channel::enable_write()
{
    events_ |= kWriteEvent;
}

void Channel::disable_write()
{
    events_ &= ~kWriteEvent;
}

void Channel::handle_event()
{
    if ((revents_ & kCloseEvent) && close_callback_) {
        close_callback_();
    }
    if ((revents_ & kErrorEvent) && error_callback_) {
        error_callback_();
    }
    if ((revents_ & kReadEvent) && read_callback_) {
        read_callback_();
    }
    if ((revents_ & kWriteEvent) && write_callback_) {
        write_callback_();
    }
}
