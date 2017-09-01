cmake_minimum_required(VERSION 2.8)
enable_testing()
project(periortree)

include_directories(${PROJECT_SOURCE_DIR})
add_definitions("-std=c++98 -pedantic -Wall")

add_subdirectory(test)
