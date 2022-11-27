cmake_minimum_required(VERSION 3.18)
project(untitled C)

set(CMAKE_C_STANDARD 99)

add_executable(untitled main.c)

target_link_libraries(untitled m)
target_link_libraries(untitled pthread)
