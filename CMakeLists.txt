cmake_minimum_required(VERSION 3.21)
project(shadows)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

add_executable(shadows
        shadows_calc/shadows.cpp
        shadows_calc/solar.hpp shadows_calc/solar.cpp
        shadows_calc/grid.hpp shadows_calc/grid.cpp
        shadows_calc/bd.hpp)
