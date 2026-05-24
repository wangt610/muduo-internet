cmake_minimum_required(VERSION 3.6)
project(muduo)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(${PROJECT_SOURCE_DIR}/boost)
include_directories(${PROJECT_SOURCE_DIR}/Log)
add_executable(TimestampTest
    Log/TimeStamp.cpp
    Log/Logger.cpp
    main.cpp
)
target_include_directories(TimestampTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Log)