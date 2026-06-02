cmake_minimum_required(VERSION 3.6)
project(muduo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add_executable(TimestampTest
#     main.cpp
    # Log/TimeStamp.cpp
    # Log/Logger.cpp
    # Log/AppendFile.cpp
    # Log/Asynclogging.cpp
    # Log/LogFile.cpp
#     muduo/base/buffer.cpp
#     muduo/base/InetAddress.cpp
#     muduo/base/Socket.cpp
#     muduo/base/Channel.cpp
# )

add_executable(main
 main.cpp 
     Log/TimeStamp.cpp
    Log/Logger.cpp
    Log/AppendFile.cpp
    Log/Asynclogging.cpp
    Log/LogFile.cpp
)

target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Log
    ${CMAKE_CURRENT_SOURCE_DIR}/muduo/base
    ${CMAKE_CURRENT_SOURCE_DIR}/threadpool
)

if(WIN32)
    target_link_libraries(main ws2_32)
endif()
