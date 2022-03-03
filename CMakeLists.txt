cmake_minimum_required(VERSION 3.21)
project(shadows)

set(CMAKE_CXX_STANDARD 14)

add_executable(shadows
        shadows_calc/shadows.cpp shadows_calc/triangulation.hpp shadows_calc/solar.hpp shadows_calc/csv.hpp shadows_calc/grid.hpp)
