cmake_minimum_required(VERSION 3.5)
project(Project1_S16_K23a)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS_DISTRIBUTION "-o3")

set(SOURCE_FILES main.c buffer.c buffer.h index.c index.h search.c search.h hash.c hash.h cc.c cc.h scc.c scc.h grail.c grail.h thread.h)
add_executable(Project1_S16_K23a ${SOURCE_FILES})