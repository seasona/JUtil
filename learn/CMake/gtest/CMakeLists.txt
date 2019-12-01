cmake_minimum_required(VERSION 3.1)
project(gtest)

add_executable(a.out gtest.cpp)
set_target_properties(a.out PROPERTIES COMPILE_FLAGS -pthread LINK_FLAGS -pthread)
target_link_libraries(a.out gtest)