#ifndef INETADDRESS_HPP
#define INETADDRESS_HPP
/*
 * InetAddress类：封装IP地址和端口，提供解析和格式化功能
 * 设计目标：
 * 1. 支持IPv4和IPv6地址
 * 2. 提供从字符串解析和转换为字符串的接口
 * 3. 内部使用sockaddr_storage存储地址，兼容不同协议族
 */
#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <string>
// 跨平台网络头文件
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

// 网络地址封装类：IP + Port
// 内部封装 sockaddr_in 结构，供 socket bind/connect 使用
class InetAddress
{
public:
    // 构造函数1：用于服务端监听（仅端口，IP默认0.0.0.0）
    explicit InetAddress(uint16_t port = 0);

    // 构造函数2：用于客户端连接（IP字符串 + 端口）
    InetAddress(const std::string& ip, uint16_t port);

    // 构造函数3：从系统 sockaddr 结构体构造
    InetAddress(const struct sockaddr_in& addr);

    // 获取IP字符串（如 127.0.0.1）
    std::string to_ip() const;

    // 获取IP:端口字符串（如 127.0.0.1:8080）
    std::string to_ip_port() const;

    // 获取端口号（主机字节序）
    uint16_t port() const;

    // 获取底层 sockaddr 指针（传给 bind/accept）
    const struct sockaddr* sock_addr() const;

    // 设置底层地址（从accept获取新地址时用）
    void set_sock_addr(const struct sockaddr_in& addr);

private:
    // Linux/Windows 通用网络地址结构体
    struct sockaddr_in addr_;
};
#endif // INETADDRESS_HPP