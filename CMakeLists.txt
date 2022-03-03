cmake_minimum_required(VERSION 3.21)
project(shadows)

set(CMAKE_CXX_STANDARD 14)

add_executable(shadows
        shadows_calc/shadows.cpp shadows_calc/delaunator.hpp)
