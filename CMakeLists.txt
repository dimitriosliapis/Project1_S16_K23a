cmake_minimum_required(VERSION 3.5)
project(anaptiksi)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c buffer.c buffer.h index.c index.h)
add_executable(anaptiksi ${SOURCE_FILES})