cmake_minimum_required(VERSION 3.2)
project(benchmark_test)

add_executable(a.out fast_integer_to_string.cpp)

# -lpthread and -pthread is different, -lpthread just link the pthread library without
# defining the pre-defined macros, so should use -pthread at best.

# google's benchmark library must use -pthread, it will occour mistake with the -lpthread  
# so set target properties every time link library benchmark
set_target_properties(a.out PROPERTIES COMPILE_FLAGS -pthread LINK_FLAGS -pthread)
target_link_libraries(a.out benchmark)
