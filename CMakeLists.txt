cmake_minimum_required(VERSION 3.29)

project(RiscVM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE src "src/*.cpp" "include/*.hpp")
add_executable(RiscVM ${src})
target_include_directories(RiscVM PUBLIC "include")
