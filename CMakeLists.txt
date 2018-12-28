cmake_minimum_required(VERSION 2.6)

project(s5-ev)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-std=c++11 -g -DDEBUG)

set(LIB_SRC
        libs5-ev/watcher.cpp libs5-ev/watcher.h
        libs5-ev/buff.cpp libs5-ev/buff.h
        libs5-ev/common.h
        libs5-ev/bufflist.cpp libs5-ev/bufflist.h
        libs5-ev/listener.cpp libs5-ev/listener.h
        libs5-ev/tcpwatcher.cpp libs5-ev/tcpwatcher.h
        libs5-ev/tcpbuff.cpp libs5-ev/tcpbuff.h
        libs5-ev/s5server.cpp libs5-ev/s5server.h
        libs5-ev/s5listener.cpp libs5-ev/s5listener.h
        libs5-ev/s5responser.cpp libs5-ev/s5responser.h
        libs5-ev/proxy.cpp libs5-ev/proxy.h)

add_library(s5-ev STATIC ${LIB_SRC})
link_libraries(s5-ev ev)
add_executable(s5-server server/main.cpp)
