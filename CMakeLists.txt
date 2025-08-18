cmake_minimum_required(VERSION 3.10)
project(LinQ VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/await:strict /utf-8)
else()
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

add_library(LinQ INTERFACE)
target_include_directories(LinQ INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(example examples/main.cpp)
target_link_libraries(example PRIVATE LinQ)