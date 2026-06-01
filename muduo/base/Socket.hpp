#ifndef SOCKET_HPP
#define SOCKET_HPP

// 必须放最前面
#define WIN32_LEAN_AND_MEAN
#define _WINSOCKAPI_

#include <winsock2.h>
#include "InetAddress.hpp"

// Socket 封装：负责创建、绑定、监听、设置非阻塞
class Socket {
public:
    explicit Socket(SOCKET sockfd)
        : sockfd_(sockfd)
    {}

    ~Socket();

    // 禁用拷贝
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    // 绑定地址
    void bind_address(const InetAddress& local_addr);

    // 监听
    void listen();

    // 接受连接
    SOCKET accept(InetAddress* peer_addr);

    // 设置非阻塞
    void set_non_blocking();

    // 设置端口复用
    void set_reuse_addr(bool on);

    // 获取原生 socket
    SOCKET fd() const { return sockfd_; }

private:
    SOCKET sockfd_;  // 原生 socket 句柄
};
#endif // SOCKET_HPP