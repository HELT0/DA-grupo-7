cmake_minimum_required(VERSION 4.1)
project(Projeto)

set(CMAKE_CXX_STANDARD 20)

add_executable(Projeto src/main.cpp
        data_structures/Graph.h
        src/backend.h
        src/backend.cpp)
