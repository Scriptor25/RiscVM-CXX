cmake_minimum_required(VERSION 3.29)

project(RiscVM)

set(ELFIO_BUILD_EXAMPLES OFF)
set(ELFIO_BUILD_TESTS, OFF)
add_subdirectory(deps/elfio)

option(RISCVM_BUILD_EXE "Specify if the main executable should be build" ${PROJECT_IS_TOP_LEVEL})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE src "src/lib/*.cpp" "include/*.hpp")
add_library(RiscVM ${src})
target_include_directories(RiscVM PUBLIC "include")

if (${RISCVM_BUILD_EXE})
    file(GLOB_RECURSE src "src/riscvm/*.cpp" "include/*.hpp")
    add_executable(RVM ${src})
    target_include_directories(RVM PRIVATE "include")
    target_link_libraries(RVM PRIVATE RiscVM elfio::elfio)
endif ()
