cmake_minimum_required(VERSION 3.7.0)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -pedantic -Werror)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -flto")

project(harmoniser_pedal VERSION 1.0 LANGUAGES CXX)

add_subdirectory(src)