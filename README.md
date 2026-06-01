# Muduo 网络库极简整体架构
## 第一部分 Log日志
### 模块1 timestamp时间戳
``` 
timestamp时间戳 
作用：高精度的时间封装 统一系统时间获取
设计详解 继承copyable 和 type类
copyable:空基类 标记类为值类型
Types.h：定义int64_t/string等基础类型别名
输出统一格式
```
### 模块2 LogLevel日志等级
```
TRACE：最详细的信息，通常用于调试。
DEBUG：调试信息，开发和调试时使用。
INFO：普通运行信息，系统正常运行时的重要事件。
WARN：警告信息，表示潜在问题但不影响程序运行。
ERROR：错误信息，程序出现错误但还能继续运行。
FATAL：严重错误，通常会导致程序终止。
```
### 模块3 日志消息组成
```
日志消息通常由以下部分组成：
1. 时间戳：记录日志产生的精确时间。
2. 日志等级：如 TRACE、DEBUG、INFO、WARN、ERROR、FATAL。
3. 线程ID：产生日志的线程编号，便于多线程环境下排查问题。
4. 源文件名与行号：日志产生的代码位置，便于定位。
5. 日志内容：具体的日志信息。

示例格式：
2026-05-24 12:34:56.789 [INFO] [Thread 1] main.cpp:42 - 服务器启动成功
```
### 模块4 日志流操作
```
日志流操作是指通过类似C++标准流（如std::cout）的方式，使用<<操作符将各种类型的数据流式写入日志。
优点：
1. 语法简洁，易于使用。
2. 支持多种数据类型的拼接输出。
3. 可链式调用，便于组合复杂日志内容。

示例用法：
LOG_INFO << "服务器启动，端口: " << port << ", 状态: " << status;

实现方式：
重载operator<<，将不同类型的数据追加到日志缓冲区，最终在日志对象析构时统一输出。
```
### 模块5 写文件类型
```
日志支持的写入类型：
1. 普通文本日志文件（.log）
2. 按日期或大小自动切分的滚动日志文件
3. 支持标准输出/标准错误输出
4. 可扩展为二进制日志或系统日志

实现方式：
通过文件流或自定义文件写入类，将日志内容高效写入目标文件，并支持自动切分和归档。
```
### 模块6 日志文件写刷盘操作
```
日志刷盘操作是指将内存中的日志缓冲区内容写入磁盘文件，确保日志数据持久化。

常见刷盘策略：
1. 定时刷盘：每隔固定时间（如1秒）将缓冲区内容写入文件。
2. 条件刷盘：当缓冲区数据量达到阈值时自动刷盘。
3. 主动刷盘：关键日志（如FATAL）或程序退出时强制刷盘。

实现方式：
通常采用专用后台线程定时或按需将日志缓冲写入文件，提升性能并降低磁盘IO压力。
可通过fflush、fsync等系统调用实现数据落盘。
```

### 模块7 异步刷盘 避免阻塞主线程
```
异步刷盘是指日志写入操作不在主线程直接进行磁盘IO，而是将日志内容先写入内存缓冲区，由专用后台线程异步写入文件。

优点：
1. 避免主线程阻塞，提高系统吞吐量。
2. 日志写入高效，适合高并发场景。

实现原理：
1. 主线程只负责将日志内容追加到缓冲区，速度极快。
2. 后台线程定期或按需将缓冲区内容写入磁盘。
3. 采用双缓冲或环形缓冲区，减少锁竞争。

典型实现：
主线程与后台线程通过线程安全队列或条件变量协作，保证日志数据及时落盘且主线程无感知。
```
## 第二部分 Threadpool 线程池模块
### 主要介绍
```
线程池 = 预先创建好的一组线程 + 任务队列
不反复创建 / 销毁线程，提升性能、降低资源消耗
控制并发数，防止系统被大量线程压垮
统一管理线程生命周期

线程本身是操作系统重量级资源，创建 / 销毁都要走内核、分配 / 回收内存、触发上下文切换，每一步都有固定开销
```
### 模块1 FiexdThreadPool
```
固定大小的线程池：预先分配一组固定大小的线程池数量，每当有任务到达时，提交到线程池，启动一个线程来执行任务。

```
### 模块2 CachedThreadPool
### 模块3 WerkStealingPool
### 模块4 ScheduledThreadPool
## 第三部分 Buffer缓冲区
```
TCP粘包：发送方多次发送的数据被对方一次性接受
专门用于TCP 粘包处理、收发数据缓存
```
## 第四部分 inetaddress地址解析与组装
### 主要介绍
```
InetAddress 封装 IPv4 的 sockaddr_in，在“用户可读字符串/端口”与“系统调用所需 sockaddr”之间转换。

作用：
1. 统一地址构造：监听端 INADDR_ANY + 端口，或指定 IP + 端口。
2. 统一地址展示：to_ip()、port()、to_ip_port() 供日志与调试。
3. 为 Socket::bind_address / accept 提供 sock_addr() 指针。
4. accept 后通过对端 sockaddr_in 回填（set_sock_addr），得到 peer 地址对象。

底层类型：sockaddr_in（AF_INET），字节序在构造/读取时用 htonl/htons、ntohs 处理。
```
### 模块1 仅端口构造（服务端监听）
```
InetAddress(uint16_t port)

memset 清零后设置：
  sin_family = AF_INET
  sin_addr.s_addr = htonl(INADDR_ANY)   // 0.0.0.0，监听所有网卡
  sin_port = htons(port)

典型：InetAddress address(8888);
      to_ip() → "0.0.0.0"，port() → 8888，to_ip_port() → "0.0.0.0:8888"
用于 listen_socket.bind_address(address)。
```
### 模块2 IP + 端口构造（指定主机）
```
InetAddress(const std::string& ip, uint16_t port)

sin_addr.s_addr = inet_addr(ip.c_str())
sin_port = htons(port)

典型：InetAddress client_addr("127.0.0.1", 9999);
      to_ip_port() → "127.0.0.1:9999"
```
### 模块3 从 sockaddr_in 构造 / 回填
```
InetAddress(const sockaddr_in& addr)     拷贝已有内核地址结构
void set_sock_addr(const sockaddr_in&)   accept 成功后写入对端地址（供 Socket::accept 使用）

流程：accept 得到 sockaddr_in → peer_addr.set_sock_addr(addr) → to_ip_port() 打印新连接
```
### 模块4 地址解析与字符串组装
```
std::string to_ip() const
  inet_ntoa(addr_.sin_addr)，返回点分十进制 IP。

uint16_t port() const
  ntohs(addr_.sin_port)，返回主机字节序端口号。

std::string to_ip_port() const
  to_ip() + ":" + std::to_string(port())，如 "127.0.0.1:9999"。
```
### 模块5 sock_addr 供系统调用
```
const sockaddr* sock_addr() const
  返回 (const sockaddr*)&addr_，长度配合 sizeof(sockaddr_in) 用于 bind。

Socket 侧：bind(sockfd_, local_addr.sock_addr(), sizeof(sockaddr_in))
```
### 典型用法
```
// 服务端绑定
InetAddress server_addr(8888);
LOG_INFO << server_addr.to_ip_port();   // 0.0.0.0:8888
listen_socket.bind_address(server_addr);

// 指定 IP
InetAddress client_addr("127.0.0.1", 9999);

// accept 后对端地址
InetAddress peer_addr;
SOCKET conn_fd = listen_socket.accept(&peer_addr);
LOG_INFO << "new connection: " << peer_addr.to_ip_port();
```
### 与第五部分 Socket 的关系
```
InetAddress 管“地址是什么”；Socket 管“句柄怎么用”。
bind / accept 只通过 InetAddress 接触 sockaddr，避免业务代码直接操作 sockaddr_in 字段与字节序。
```
## 第五部分 Socket基础封装（创建、绑定、监听、非阻塞）
### 主要介绍
```
Socket 类对 Winsock 的 SOCKET 句柄做 RAII 封装，配合 InetAddress 完成服务端常用流程：
创建（外部 socket()）→ 端口复用 → 绑定 → 监听 → 非阻塞 accept。

设计要点：
1. 只封装“已有 fd”，不在类内调用 socket() 创建，创建仍由调用方完成，便于与 muduo 分层一致。
2. 禁止拷贝/赋值，句柄唯一归属，析构时 closesocket，避免泄漏与重复关闭。
3. 面向 Windows：bind/listen/accept/setsockopt/ioctlsocket，链接 ws2_32.lib。
```
### 模块1 构造与析构（RAII）
```
explicit Socket(SOCKET sockfd)  接管已创建的原生句柄，存入 sockfd_
~Socket()                        若 sockfd_ != INVALID_SOCKET 则 closesocket

拷贝构造、赋值运算符 = delete，保证一个 Socket 对象独占一个句柄。

fd() 返回底层 SOCKET，需要交给其他模块或继续封装时使用。
```
### 模块2 bind_address 绑定地址
```
void bind_address(const InetAddress& local_addr)

内部：bind(sockfd_, local_addr.sock_addr(), sizeof(sockaddr_in))
失败时输出 "bind failed"（当前为 cerr，可后续接入日志）。

典型顺序：先 set_reuse_addr(true)，再 bind_address，避免 TIME_WAIT 导致重启 bind 失败。
```
### 模块3 listen 进入监听
```
void listen()

内部：::listen(sockfd_, SOMAXCONN)，队列长度取系统允许的最大值。
失败时输出 "listen failed"。
须在 bind 成功之后调用。
```
### 模块4 accept 接受连接
```
SOCKET accept(InetAddress* peer_addr)

从监听套接字接受新连接，成功时将对端 sockaddr_in 写入 peer_addr（set_sock_addr）。
返回新连接的 SOCKET；失败返回 INVALID_SOCKET（非阻塞下常见，需循环重试）。

注意：本实现不在 accept 内自动设非阻塞，对返回的 connfd 应再包一层 Socket 并调用 set_non_blocking()。
```
### 模块5 set_non_blocking 非阻塞
```
void set_non_blocking()

ioctlsocket(sockfd_, FIONBIO, &mode)，mode = 1 开启非阻塞。
监听套接字设为非阻塞后，accept 在无连接时立即返回 INVALID_SOCKET，事件驱动循环中可 continue，不阻塞线程。
```
### 模块6 set_reuse_addr 地址复用
```
void set_reuse_addr(bool on)

setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, ...)
允许绑定处于 TIME_WAIT 的地址，开发/重启服务时常用。
```
### 典型用法（与 main 一致）
```
WSAStartup(...);
SOCKET listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
Socket listen_socket(listen_fd);

InetAddress address(8888);
listen_socket.set_reuse_addr(true);
listen_socket.bind_address(address);
listen_socket.listen();
listen_socket.set_non_blocking();

while (true) {
    InetAddress peer_addr;
    SOCKET conn_fd = listen_socket.accept(&peer_addr);
    if (conn_fd == INVALID_SOCKET) continue;

    Socket conn_socket(conn_fd);
    conn_socket.set_non_blocking();
    // peer_addr.to_ip_port() 获取对端地址
}
WSACleanup();
```
### 与第四部分 InetAddress 的关系
```
bind_address / accept 均依赖 InetAddress 提供的 sock_addr() 与对端地址回填。
Socket 不负责 IP/端口字符串解析，地址语义集中在 InetAddress，Socket 只操作句柄生命周期与系统调用。
```
## 第六部分 Channel 事件通道（一个 fd 的事件管家）
### 主要介绍
```
Channel 是 Reactor 的核心：一个 fd 对应一个 Channel，负责该 socket 上的事件与回调。

职责（四件事）：
1. 绑定一个 socket（构造时传入 fd）
2. 注册 4 种回调：可读、可写、关闭、错误
3. 通过 events() 告诉 Poller（epoll/select/WSAPoll）要监听什么
4. Poller 返回后 set_revents + handle_event()，自动调用对应回调

分工：Socket 管句柄与系统调用；Channel 管“监听什么、触发后做什么”；Poller 管“何时就绪”。
```
### 模块1 绑定 fd
```
explicit Channel(socket_t fd)

fd() 返回绑定的 socket，与 Socket::fd() 一致。
每个 listen_fd / conn_fd 各建一个 Channel，生命周期由 EventLoop 或上层容器管理。
```
### 模块2 注册四种回调
```
set_read_callback   可读：accept、recv
set_write_callback  可写：send
set_close_callback  关闭：对端 FIN / hangup
set_error_callback  错误：异常条件

类型：std::function<void()>
```
### 模块3 告诉 Poller 监听什么
```
events_   关注的事件掩码
enable_read / disable_read / enable_write / disable_write

Poller 注册时使用 channel->events()，例如：
  kReadEvent=1  kWriteEvent=2  kErrorEvent=4  kCloseEvent=8

events 变化后需由 EventLoop 重新 update 到 Poller（下一步实现 Poller 时衔接）。
```
### 模块4 事件触发后自动分发
```
revents_  Poller::poll 返回后写入：channel->set_revents(rev)
handle_event()  按顺序调用回调：
  1. close_callback（kCloseEvent）
  2. error_callback（kErrorEvent）
  3. read_callback（kReadEvent）
  4. write_callback（kWriteEvent）
```
### 典型用法
```
Channel listen_ch(listen_fd);
listen_ch.enable_read();
listen_ch.set_read_callback([&]() {
    InetAddress peer;
    SOCKET conn = listen_socket.accept(&peer);
    if (conn == INVALID_SOCKET) return;
    Channel conn_ch(conn);
    conn_ch.enable_read();
    conn_ch.set_read_callback([...]() { /* 读数据 */ });
    // poller->update_channel(&conn_ch);
});

// 事件循环中
poller.poll(timeout);
for (Channel* ch : active_channels) {
    ch->handle_event();
}
```
### 后续衔接
```
下一步：Poller 持有 Channel* 列表，poll 时把内核 revents 转成 kReadEvent 等并 set_revents。
再实现 EventLoop：loop 中 poll → 遍历 active_channels → handle_event。
```
## C++新特征pool
## 设计模式
```

```
## 编译命令
```
cmake .. 编译产物
cmake --build . --config Release
```
