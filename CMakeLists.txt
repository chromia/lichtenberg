cmake_minimum_required(VERSION 2.8.12)
project(_lichtenberg)

add_subdirectory(pybind11)

file(GLOB SOURCES
    src/*.h
    src/*.cpp
    src/model/*.h
    src/model/*.cpp
    src/util/*.h
    src/util/*.cpp
)
set(CMAKE_CXX_STANDARD 17)
pybind11_add_module(_lichtenberg ${SOURCES})
