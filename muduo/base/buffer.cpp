// Buffer.cpp
#include "buffer.hpp"

/*
 * 从socket读取数据到缓冲区
 * 逻辑：
 * 1. 先读进缓冲区可写空间
 * 2. 若缓冲区满，用栈上临时缓存接收溢出数据
 * 3. 自动追加到缓冲区并扩容
 * 跨平台：Windows下fd是SOCKET，用recv；Linux用read
 */
ssize_t Buffer::read_fd(int fd, int *saved_errno)
{
    // 栈上临时缓冲区：64KB，避免频繁堆扩容
    char extra_buf[65536];
    const int writable = writable_bytes();

    ssize_t n = 0;
#ifdef _WIN32
    // Windows：socket用recv，flag=0
    n = recv(fd, buffer_.data() + write_pos_, writable, 0);
#else
    // Linux：文件描述符用read
    n = read(fd, buffer_.data() + write_pos_, writable);
#endif

    if (n < 0)
    {
        // 读取失败：保存错误码
        *saved_errno = errno;
    }
    else if (n == 0)
    {
        // 对端关闭连接：返回0
        *saved_errno = 0;
    }
    else
    {
        // 读取成功：写指针后移
        write_pos_ += static_cast<int>(n);
        // 若缓冲区已满，读取溢出数据到临时缓存
        if (n == writable)
        {
            ssize_t extra_n = 0;
#ifdef _WIN32
            extra_n = recv(fd, extra_buf, sizeof(extra_buf), 0);
#else
            extra_n = read(fd, extra_buf, sizeof(extra_buf));
#endif
            if (extra_n > 0)
            {
                // 追加溢出数据到缓冲区（自动扩容）
                append(extra_buf, static_cast<int>(extra_n));
                n += extra_n;
            }
            else if (extra_n < 0)
            {
                *saved_errno = errno;
            }
        }
    }
    return n;
}