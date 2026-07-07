cmake_minimum_required(VERSION 3.28)
project(fileexplorer)

set(CMAKE_CXX_STANDARD 20)

add_executable(fileexplorer
        headers/touch.h
        src/touch.c
        headers/rm.h
        src/rm.c
        headers/pwd.h
        src/pwd.c
        headers/mkdir.h
        src/mkdir.c
        headers/utils.h
        src/utils.c
        headers/cd.h
        src/cd.c
        headers/cat.h
        src/cat.c
        headers/list.h
        src/list.c)
