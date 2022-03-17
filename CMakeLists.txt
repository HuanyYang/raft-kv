cmake_minimum_required(VERSION 3.21)
project(raft_kv)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(raft_kv main.cpp
        SkipList.h main.cpp)
