cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(CExt3D VERSION 0.1.0 LANGUAGES C)

find_library(MATH_LIBRARY m)
find_package(SDL2 REQUIRED)

add_executable(main main.c graphics.c images.c map.c rays.c vectors.c)
target_link_libraries(main PRIVATE SDL2::SDL2 ${MATH_LIBRARY})
