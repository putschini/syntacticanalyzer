cmake_minimum_required(VERSION 3.5.0)

project( syntacticanalyzer )

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_subdirectory(./lexicanalyzer)

file(GLOB SOURCE "./src/*.cpp")
file(GLOB HPP "./src/*.cpp")

add_library( libsyntacticanalyzer STATIC ${SOURCE}  ${HPP})
target_link_libraries(libsyntacticanalyzer PRIVATE liblexicanalyzer)
target_include_directories(libsyntacticanalyzer PUBLIC ./src/)

add_executable( syntacticanalyzer main.cpp )
target_link_libraries(syntacticanalyzer PRIVATE liblexicanalyzer libsyntacticanalyzer )
