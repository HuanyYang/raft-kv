cmake_minimum_required(VERSION 3.21)
project(raft_kv)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(raft_kv main.cc
        db/skiplist.h db/wal.h db/wal.cc db/leptdb.h db/leptdb.cc db/manifest.h)
