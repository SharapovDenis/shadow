cmake_minimum_required(VERSION 3.21)
project(shadows)

set(CMAKE_CXX_STANDARD 17)

add_executable(shadows
        shadows_calc/shadows.cpp shadows_calc/grid.hpp shadows_calc/solar.hpp)
