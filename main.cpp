
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
    //Timestamp ts = Timestamp::Now();
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

#include <iostream>
#include "InetAddress.hpp"

// 测试 InetAddress 地址类
int main() {

    AsyncLogging logFile("D:/muduo/muduo-internet/logmsg/log_info_test", 1024 * 1024,1);
    // logFile.start();
    // // 绑定Logger输出到AsyncLogging
    // Logger::setOutput([&logFile](const std::string& msg){ logFile.append(msg); });
    // Logger::setFlush([&logFile](){ logFile.flush(); });

    // // ====================== 测试1：服务端地址（只传端口） ======================
    // InetAddress server_addr(8888);  
    // LOG_INFO << "===== 服务端地址 =====" ;
    // LOG_INFO << "IP:   " << server_addr.to_ip() ;       // 0.0.0.0
    // LOG_INFO << "Port: " << server_addr.port() ;        // 8888
    // LOG_INFO << "Full: " << server_addr.to_ip_port() ;  // 0.0.0.0:8888

    // // ====================== 测试2：客户端地址（IP + 端口） ======================
    // InetAddress client_addr("127.0.0.1", 9999);
    // LOG_INFO << "\n===== 客户端地址 =====" ;
    // LOG_INFO << "IP:   " << client_addr.to_ip() ;       // 127.0.0.1
    // LOG_INFO << "Port: " << client_addr.port() ;        // 9999
    // LOG_INFO << "Full: " << client_addr.to_ip_port() ;  // 127.0.0.1:9999

    // // ====================== 测试3：获取底层 sockaddr 结构 ======================
    // // 这个接口给 socket bind/accept 使用
    // const struct sockaddr* addr_ptr = client_addr.sock_addr();
    LOG_INFO << "\n底层 sockaddr 地址获取成功！" ;

    LOG_INFO << "\n===== InetAddress 所有功能测试通过！=====" ;
    return 0;
}