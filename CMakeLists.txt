cmake_minimum_required(VERSION 2.8)
enable_testing()
project(periortree)

include_directories(${PROJECT_SOURCE_DIR})
add_definitions("-pedantic -Wall")

set(STANDARD_VERSION "-std=c++98" CACHE STRING "available version: -std=c++XX")
add_definitions(${STANDARD_VERSION})

add_subdirectory(src)
add_subdirectory(test)
