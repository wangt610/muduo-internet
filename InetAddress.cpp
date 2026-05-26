#include "InetAddress.hpp"
#include <cstring>
#include <cstdio>

#pragma comment(lib, "ws2_32.lib")

InetAddress::InetAddress(uint16_t port)
{
    LOG_INFO << "InetAddress构造函数被调用，port=" << port;
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_.sin_port = htons(port);
    LOG_INFO << "InetAddress构造完成，IP=" << to_ip() << ", Port=" << port;
}

InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
    LOG_INFO << "InetAddress构造函数被调用，ip=" << ip << ", port=" << port;
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = inet_addr(ip.c_str());
    addr_.sin_port = htons(port);
    LOG_INFO << "InetAddress构造完成，IP=" << to_ip() << ", Port=" << port;
}

InetAddress::InetAddress(const sockaddr_in& addr)
    : addr_(addr)
{}

std::string InetAddress::to_ip() const
{
    LOG_INFO << "InetAddress::to_ip()被调用，IP=" << inet_ntoa(addr_.sin_addr);
    return inet_ntoa(addr_.sin_addr);
    LOG_INFO << "InetAddress::to_ip()完成，IP=" << inet_ntoa(addr_.sin_addr);
}

std::string InetAddress::to_ip_port() const
{
    LOG_INFO << "InetAddress::to_ip_port()被调用，IP=" << inet_ntoa(addr_.sin_addr) << ", Port=" << port();
    std::string res = to_ip();
    res += ":" + std::to_string(port());
    LOG_INFO << "InetAddress::to_ip_port()完成，IP:Port=" << res;
    return res;
}

uint16_t InetAddress::port() const
{
    return ntohs(addr_.sin_port);
}

const sockaddr* InetAddress::sock_addr() const
{
    return (const sockaddr*)&addr_;
}