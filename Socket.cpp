#include "Socket.hpp"
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

Socket::~Socket()
{
    if (sockfd_ != INVALID_SOCKET) {
        closesocket(sockfd_);
    }
}

// 绑定地址
void Socket::bind_address(const InetAddress& local_addr)
{
    int ret = bind(sockfd_, local_addr.sock_addr(), sizeof(sockaddr_in));
    if (ret == SOCKET_ERROR) {
        std::cerr << "bind failed\n";
    }
}

// 监听
void Socket::listen()
{
    int ret = ::listen(sockfd_, SOMAXCONN);
    if (ret == SOCKET_ERROR) {
        std::cerr << "listen failed\n";
    }
}

// 接受新连接
SOCKET Socket::accept(InetAddress* peer_addr)
{
    sockaddr_in addr;
    int addr_len = sizeof(addr);

    SOCKET connfd = ::accept(sockfd_, (sockaddr*)&addr, &addr_len);

    // 设置接受的 socket 为非阻塞
    if (connfd != INVALID_SOCKET) {
        peer_addr->set_sock_addr(addr);
    }

    return connfd;
}

// 设置非阻塞
void Socket::set_non_blocking()
{
    u_long mode = 1;
    ioctlsocket(sockfd_, FIONBIO, &mode);
}

// 设置端口复用
void Socket::set_reuse_addr(bool on)
{
    int opt = on ? 1 : 0;
    setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
}