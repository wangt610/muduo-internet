#include "TimeStamp.hpp"
#include <iostream>
#include"LogLevel.hpp"
#include "LogMessage.hpp"
#include "Logger.hpp"


int main()
{
    Timestamp ts = Timestamp::Now();
    // std::cout << "LogLevelName[LogLevel::INFO]: " << levelStr[INFO] << std::endl;
    // std::cout << "toString: " << ts.toString() << std::endl;
    // std::cout << "formatted: " << ts.toFormattedString(true) << std::endl;
    // std::cout << "filetime: " << ts.toFormattedFile() << std::endl;

    // std::cout << "Testing LogMessage: " << std::endl;
    // LogMessage logMsg(ts, INFO, "This is a log message", __LINE__, __func__);
    // logMsg << "Additional info: " << 42;
    // std::cout << logMsg.getmsg() << std::endl;

    std::cout << "Testing Logger: " << std::endl;
    LOG_INFO << "This is an info message" ;
    LOG_ERROR << "This is an error message with value: " << 123;


    return 0;
}