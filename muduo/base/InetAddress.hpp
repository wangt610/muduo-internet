#ifndef INETADDRESS_HPP
#define INETADDRESS_HPP
/*
 * InetAddress类：封装IP地址和端口，提供解析和格式化功能
 * 设计目标：
 * 1. 支持IPv4和IPv6地址
 * 2. 提供从字符串解析和转换为字符串的接口
 * 3. 内部使用sockaddr_storage存储地址，兼容不同协议族
 */
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#include <string>
#include"Asynclogging.hpp" 

class InetAddress
{
public:
    explicit InetAddress(uint16_t port = 0);
    InetAddress(const std::string& ip, uint16_t port);
    explicit InetAddress(const sockaddr_in& addr);

    std::string to_ip() const;
    std::string to_ip_port() const;
    uint16_t port() const;

    const sockaddr* sock_addr() const;
    void set_sock_addr(const sockaddr_in& addr) { addr_ = addr; }

private:
    sockaddr_in addr_;
};
#endif // INETADDRESS_HPP