cmake_minimum_required(VERSION 3.21)
project(shadows)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lsqlite3")

add_executable(shadows
        shadows_calc/shadows.cpp
#        shadows_calc/structure.cc
        shadows_calc/solar.hpp shadows_calc/solar.cpp
        shadows_calc/grid.hpp shadows_calc/grid.cpp
        shadows_calc/db.hpp shadows_calc/db.cpp
        )

include_directories(/usr/include)
link_directories(/usr/lib)
target_link_libraries(shadows sqlite3)
