cmake_minimum_required(VERSION 3.9)
project(memorypool VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED OFF)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(MemPool)

aux_source_directory(. DIR_SRCS)

add_executable(main ${DIR_SRCS})

target_link_libraries(main MemPool)