#include "InetAddress.hpp"
#include <cstring>  // 引入 memset

// ------------------- 构造函数 -------------------
// 服务端监听：仅端口，IP = INADDR_ANY (0.0.0.0)
InetAddress::InetAddress(uint16_t port)
{
    // 清空结构体（替代 bzero）
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;         // IPv4
    addr_.sin_addr.s_addr = htonl(INADDR_ANY); // 监听所有网卡
    addr_.sin_port = htons(port);       // 端口转网络字节序
}

// 客户端连接：指定IP字符串 + 端口
InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    // 字符串IP → 网络字节序IP
    inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
    addr_.sin_port = htons(port);
}

// 从系统结构体构造
InetAddress::InetAddress(const struct sockaddr_in& addr)
  : addr_(addr)
{}

// ------------------- 转换接口 -------------------
// 网络IP → 字符串IP
std::string InetAddress::to_ip() const
{
    char buf[64] = {0};
    // 网络字节序 → 点分十进制字符串
    inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof(buf));
    return buf;
}

// 返回 "ip:port" 格式字符串
std::string InetAddress::to_ip_port() const
{
    std::string res = to_ip();
    res += ":";
    res += std::to_string(port());
    return res;
}

// 获取主机字节序端口
uint16_t InetAddress::port() const
{
    return ntohs(addr_.sin_port);
}

// ------------------- 底层接口 -------------------
// 返回 sockaddr* 指针（bind/accept 用）
const struct sockaddr* InetAddress::sock_addr() const
{
    return reinterpret_cast<const struct sockaddr*>(&addr_);
}

// 设置地址（accept 新连接时使用）
void InetAddress::set_sock_addr(const struct sockaddr_in& addr)
{
    addr_ = addr;
}