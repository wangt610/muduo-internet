#ifndef BUFFER_HPP
#define BUFFER_HPP
#pragma once

/*
 * 网络库专用动态缓冲区（仿 muduo 精简版）
 * 核心设计：
 * 1. 底层用 vector<char> 管理内存，自动扩容
 * 2. 双指针：read_pos_ / write_pos_ 划分三区
 *    +----------------+----------------+----------------+
 *    |  prependable   |    readable    |    writable    |
 *    |   (已读空间)    |   (待处理数据)  |   (可写空间)   |
 *    +----------------+----------------+----------------+
 *    0             read_pos_       write_pos_      size()
 * 3. 支持自动扩容、数据前移复用空间、跨平台兼容
 */

#include <vector>
#include <cstring>
#include <cassert>

// 跨平台类型兼容
#ifdef _WIN32
#include <winsock2.h>
using ssize_t = int;
#else
#include <unistd.h>
#endif

class Buffer {
public:
    // 常量定义：预留头部空间 + 初始缓冲区大小
    static const int kCheapPrepend = 8;   // 预留8字节，方便后续加协议头
    static const int kInitialSize = 1024; // 初始可写空间1KB

    // 构造函数：初始化缓冲区，读写指针都指向预留空间末尾
    explicit Buffer(int init_size = kInitialSize)
        : buffer_(kCheapPrepend + init_size),
          read_pos_(kCheapPrepend),
          write_pos_(kCheapPrepend) {}

    // -------------------------- 只读接口 --------------------------
    // 获取可读数据字节数（待处理数据长度）
    int readable_bytes() const {
        return write_pos_ - read_pos_;
    }

    // 获取可写空间字节数（剩余可写入的空间）
    int writable_bytes() const {
        return static_cast<int>(buffer_.size()) - write_pos_;
    }

    // 获取预留空间字节数（读指针之前的空间，可用于前置写）
    int prependable_bytes() const {
        return read_pos_;
    }

    // 获取可读数据起始地址（peek：偷看，不移动读指针）
    char* peek() {
        return buffer_.data() + read_pos_;
    }

    const char* peek() const {
        return buffer_.data() + read_pos_;
    }

    // -------------------------- 数据读取接口 --------------------------
    // 取出n字节数据（移动读指针）
    void retrieve(int n) {
        // 确保n不超过可读长度
        assert(n <= readable_bytes());
        if (n < readable_bytes()) {
            // 只读部分数据：读指针后移n
            read_pos_ += n;
        } else {
            // 读完所有数据：重置读写指针到预留位置
            retrieve_all();
        }
    }

    // 取出所有可读数据
    void retrieve_all() {
        read_pos_ = kCheapPrepend;
        write_pos_ = kCheapPrepend;
    }

    // -------------------------- 数据写入接口 --------------------------
    // 追加数据到缓冲区末尾（核心写接口）
    void append(const char* data, int len) {
        // 确保有足够可写空间，不足则扩容或整理
        ensure_writable(len);
        // 拷贝数据到写指针位置
        std::copy(data, data + len, buffer_.data() + write_pos_);
        // 写指针后移len
        write_pos_ += len;
    }

    // 前置写入数据（在可读数据前写，用于加协议头）
    void prepend(const char* data, int len) {
        // 确保预留空间足够
        assert(len <= prependable_bytes());
        // 读指针前移len
        read_pos_ -= len;
        // 拷贝数据到新的读指针位置
        std::copy(data, data + len, buffer_.data() + read_pos_);
    }

    // -------------------------- 网络IO接口 --------------------------
    // 从socket读取数据到缓冲区（跨平台：Windows用recv，Linux用read）
    ssize_t read_fd(int fd, int* saved_errno);

private:
    // 确保至少有len字节可写空间
    void ensure_writable(int len) {
        if (writable_bytes() >= len) {
            // 空间足够，无需操作
            return;
        }
        // 空间不足：扩容或数据前移复用
        make_space(len);
    }

    // 空间整理/扩容：优先复用前部空间，不足则扩容
    void make_space(int len) {
        // 计算总空闲空间（预留+可写）
        int total_free = prependable_bytes() + writable_bytes();
        if (total_free >= len) {
            // 总空间足够：把可读数据前移，复用前部空间
            int readable = readable_bytes();
            // 数据拷贝到预留空间后
            std::copy(peek(), peek() + readable, buffer_.data() + kCheapPrepend);
            // 重置读写指针
            read_pos_ = kCheapPrepend;
            write_pos_ = kCheapPrepend + readable;
        } else {
            // 总空间不足：直接扩容到所需大小
            buffer_.resize(write_pos_ + len);
        }
    }

    std::vector<char> buffer_; // 底层存储：连续内存
    int read_pos_;              // 读指针：可读数据起始位置
    int write_pos_;             // 写指针：可写空间起始位置
};

#endif // BUFFER_HPP