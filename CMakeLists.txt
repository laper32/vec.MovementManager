cmake_minimum_required(VERSION 3.16)
project(MovementManager)

set(CMAKE_CXX_STANDARD 20)
include(smsdk_ext-cmake/smsdk_ext.cmake)

add_subdirectory(smcppsdk)
add_subdirectory(src)