cmake_minimum_required(VERSION 3.16)
project(CLox)

set(CMAKE_CXX_STANDARD 14)

add_executable(CLox main.cpp common.h chunk.h chunk.cpp debug.h debug.cpp value.h value.cpp vm.h vm.cpp compiler.h compiler.cpp scanner.h scanner.cpp parser.h parser.cpp)