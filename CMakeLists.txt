cmake_minimum_required(VERSION 3.31)
project(brick_breaker)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(HelloECS)

add_executable(brick_breaker main.cpp)
target_link_libraries(brick_breaker PRIVATE HelloECS)