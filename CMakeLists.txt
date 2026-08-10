cmake_minimum_required(VERSION 3.25)

project(sand)
include(FetchContent)

add_executable(sand main.cpp)
target_compile_features(sand PRIVATE cxx_std_26)
target_compile_options(sand PRIVATE -freflection)
target_compile_options(sand PRIVATE -O3 -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)

FetchContent_Declare(xieite GIT_REPOSITORY https://github.com/Eczbek/xieite GIT_TAG main)
FetchContent_MakeAvailable(xieite)
target_link_libraries(sand PRIVATE xieite)
