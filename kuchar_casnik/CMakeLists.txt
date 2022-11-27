cmake_minimum_required(VERSION 3.18)
project(kuchar_casnik C)

set(CMAKE_C_STANDARD 99)

add_executable(kuchar_casnik main.c)
target_link_libraries(kuchar_casnik pthread)
