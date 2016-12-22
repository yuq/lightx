cmake_minimum_required(VERSION 2.6)
project(lightx)

include_directories(include)

find_package(PkgConfig)
pkg_check_modules(PC_XPROTO xproto)
pkg_check_modules(PC_GLPROTO glproto)

add_executable(lightx main.c socket.c dispatch.c server.c client.c xproto.c glx.c)
