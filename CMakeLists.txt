cmake_minimum_required(VERSION 3.27)
project(q C)

set(CMAKE_C_STANDARD 11)

add_executable(q main.c
        lexer.c
        lexer.h
        hash.c
        hash.h
        macro.c
        macro.h
)
