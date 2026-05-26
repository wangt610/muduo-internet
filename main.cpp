
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
    // std::cout << "LogLevelName[LogLevel::INFO]: " << levelStr[INFO] << std::endl;
    // std::cout << "toString: " << ts.toString() << std::endl;
    // std::cout << "formatted: " << ts.toFormattedString(true) << std::endl;
    // std::cout << "filetime: " << ts.toFormattedFile() << std::endl;

    // std::cout << "Testing LogMessage: " << std::endl;
    // LogMessage logMsg(ts, INFO, "This is a log message", __LINE__, __func__);
    // logMsg << "Additional info: " << 42;
    // std::cout << logMsg.getmsg() << std::endl;

    // std::cout << "Testing Logger: " << std::endl;
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

// 测试用例：LOG_INFO写日志到异步磁盘
int main() {
    AsyncLogging logFile("D:/muduo/muduo-internet/logmsg/log_info_test", 1024 * 1024,1);
    logFile.start();
    // 绑定Logger输出到AsyncLogging
    Logger::setOutput([&logFile](const std::string& msg){ logFile.append(msg); });
    Logger::setFlush([&logFile](){ logFile.flush(); });
    // 多线程写日志
    const int threadCount = 4;
    const int logsPerThread = 1000;
    std::vector<std::thread> threads;
    for (int t = 0; t < threadCount; ++t) {
        threads.emplace_back([t, logsPerThread]() {
            for (int i = 0; i < logsPerThread; ++i) {
                LOG_INFO << "Thread " << t << " log " << i;
                LOG_ERROR << "Thread " << t << " error log " << i;
            }
        });
    }
    for (auto& th : threads) th.join();
    logFile.flush();
    logFile.stop();
    //std::cout << "LOG_INFO写日志测试完成，请检查 logmsg/log_info_test* 文件内容。" << std::endl;
    return 0;
}