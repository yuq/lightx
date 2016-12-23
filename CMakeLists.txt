cmake_minimum_required(VERSION 2.6)
project(lightx)

include_directories(include)

find_package(PkgConfig)
pkg_check_modules(PC_XPROTO xproto)
pkg_check_modules(PC_GLPROTO glproto)
pkg_check_modules(PC_DRI3PROTO dri3proto)
pkg_check_modules(PC_PRESENTPROTO presentproto)

add_executable(lightx
  main.c socket.c dispatch.c server.c client.c xproto.c glx.c 
  dri3.c present.c)
