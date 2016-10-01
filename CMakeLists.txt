cmake_minimum_required(VERSION 2.6)
project(lightx)
include_directories(include)
add_executable(lightx main.c socket.c dispatch.c server.c client.c)
