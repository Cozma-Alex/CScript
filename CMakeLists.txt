cmake_minimum_required(VERSION 3.26)
project(Compiler_CDash)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_ASM_NASM_COMPILER "/usr/bin/nasm")

add_executable(Compiler src/main.cpp
        src/tokenization.h)
