cmake_minimum_required(VERSION 3.25)

project(sand)
include(FetchContent)

add_executable(${PROJECT_NAME} main.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_26)
target_compile_options(${PROJECT_NAME} PRIVATE -Wpedantic -Wall -Wextra -Wconversion -Wsign-conversion)

FetchContent_Declare(xieite GIT_REPOSITORY "https://github.com/Eczbek/xieite" GIT_TAG "main")
FetchContent_MakeAvailable(xieite)
target_link_libraries(${PROJECT_NAME} PRIVATE xieite)
