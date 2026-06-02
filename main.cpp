
// #include <iostream>
// #include "Asynclogging.hpp"

// AsyncLogging logFile("D:\\muduo\\muduo-internet\\logmsg\\wangt", 1024 * 1024 * 10, 0.01);

// int main()
// {
// Logger::setOutput(logOutput);
// Logger::setFlush(logFlush);
// for(int i=0;i<10000;i++){
//     LOG_INFO<<"hello world "<<i;
// }
// Timestamp ts = Timestamp::Now();
// LOG_INFO << "LogLevelName[LogLevel::INFO]: " << levelStr[INFO] ;
// LOG_INFO << "toString: " << ts.toString() ;
// LOG_INFO << "formatted: " << ts.toFormattedString(true) ;
// LOG_INFO << "filetime: " << ts.toFormattedFile() ;

// LOG_INFO << "Testing LogMessage: " ;
// LogMessage logMsg(ts, INFO, "This is a log message", __LINE__, __func__);
// logMsg << "Additional info: " << 42;
// LOG_INFO << logMsg.getmsg() ;

// LOG_INFO << "Testing Logger: " ;
// LOG_INFO << "This is an info message" ;
// LOG_ERROR << "This is an error message with value: " << 123;

//     return 0;
// }

#include "Asynclogging.hpp"
#include "Logger.hpp"
#include <thread>
#include <vector>
#include <string>
#include <iostream>

// // 测试用例：LOG_INFO写日志到异步磁盘
// int main() {
// AsyncLogging logFile("D:/muduo/muduo-internet/logmsg/log_info_test", 1024 * 1024,1);
// logFile.start();
// // 绑定Logger输出到AsyncLogging
// Logger::setOutput([&logFile](const std::string& msg){ logFile.append(msg); });
// Logger::setFlush([&logFile](){ logFile.flush(); });
//     // 多线程写日志
//     const int threadCount = 4;
//     const int logsPerThread = 1000;
//     std::vector<std::thread> threads;
//     for (int t = 0; t < threadCount; ++t) {
//         threads.emplace_back([t, logsPerThread]() {
//             for (int i = 0; i < logsPerThread; ++i) {
//                 LOG_INFO << "Thread " << t << " log " << i;
//                 LOG_ERROR << "Thread " << t << " error log " << i;
//             }
//         });
//     }
//     for (auto& th : threads) th.join();
// logFile.flush();
// logFile.stop();
//     //LOG_INFO << "LOG_INFO写日志测试完成，请检查 logmsg/log_info_test* 文件内容。" ;
//     return 0;
// }
// #include <iostream>
// #include "InetAddress.hpp"

// // 测试 InetAddress 地址类
// int main()
// {

    // AsyncLogging logFile("D:/muduo/muduo-internet/logmsg/log_info_test", 1024 * 1024, 1);
    // logFile.start();
    // // 绑定Logger输出到AsyncLogging
    // Logger::setOutput([&logFile](const std::string &msg)
    //                   { logFile.append(msg); });
    // Logger::setFlush([&logFile]()
    //                  { logFile.flush(); });

//     // ====================== 测试1：服务端地址（只传端口） ======================
//     InetAddress server_addr(8888);
//     LOG_INFO << "===== 服务端地址 =====";
//     LOG_INFO << "IP:   " << server_addr.to_ip();      // 0.0.0.0
//     LOG_INFO << "Port: " << server_addr.port();       // 8888
//     LOG_INFO << "Full: " << server_addr.to_ip_port(); // 0.0.0.0:8888

//     // ====================== 测试2：客户端地址（IP + 端口） ======================
//     InetAddress client_addr("127.0.0.1", 9999);
//     LOG_INFO << "\n===== 客户端地址 =====";
//     LOG_INFO << "IP:   " << client_addr.to_ip();      // 127.0.0.1
//     LOG_INFO << "Port: " << client_addr.port();       // 9999
//     LOG_INFO << "Full: " << client_addr.to_ip_port(); // 127.0.0.1:9999

//     // ====================== 测试3：获取底层 sockaddr 结构 ======================
//     // 这个接口给 socket bind/accept 使用
//     const struct sockaddr *addr_ptr = client_addr.sock_addr();
    // logFile.flush();
    // logFile.stop();
//     LOG_INFO << "底层 sockaddr 地址获取成功 ";
//     LOG_INFO << "===== InetAddress 所有功能测试通过！=====";
//     return 0;
// }

// #define WIN32_LEAN_AND_MEAN
// #define _WINSOCKAPI_

// #include <iostream>
// #include "InetAddress.hpp"
// #include "Socket.hpp"

// int main() {

//     // 初始化 Winsock
//     WSADATA wsaData;
//     WSAStartup(MAKEWORD(2, 2), &wsaData);

//     // 创建监听 socket
//     SOCKET listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//     Socket listen_socket(listen_fd);

//     // 绑定 8888 端口
//     InetAddress address(8888);
//     listen_socket.set_reuse_addr(true);
//     listen_socket.bind_address(address);
//     listen_socket.listen();
//     listen_socket.set_non_blocking();

//     LOG_INFO << "listen on 8888\n";

//     // 循环接受连接（简单演示）
//      InetAddress peer_addr;
//     while (true) {
//         SOCKET conn_fd = listen_socket.accept(&peer_addr);

//         if (conn_fd == INVALID_SOCKET) {
//             continue;
//         }

//         LOG_INFO<< "new connection: " << peer_addr.to_ip_port();
//         Socket conn_socket(conn_fd);
//         conn_socket.set_non_blocking();
//     }

    // logFile.flush();
    // logFile.stop();
//     WSACleanup();
//     return 0;
// }

#include "FixedThreadPool.hpp"

int main() {
      AsyncLogging logFile("D:/muduo/muduo-internet/logmsg/log_info_test", 1024 * 1024, 1);
    logFile.start();
    // 绑定Logger输出到AsyncLogging
    Logger::setOutput([&logFile](const std::string &msg)
                      { logFile.append(msg); });
    Logger::setFlush([&logFile]()
                     { logFile.flush(); });
    FixedThreadPool pool(4); // 创建一个包含4个线程的线程池

    // 提交一些任务到线程池
    for (int i = 0; i < 10; ++i) {
        pool.AddTask([i]() {
            LOG_INFO << "Task " << i << " is running in thread " 
                      << std::this_thread::get_id() ;
        });
    }

    // 等待一段时间让任务执行完毕
    std::this_thread::sleep_for(std::chrono::seconds(2));

      logFile.flush();
    logFile.stop();
    return 0;
}


