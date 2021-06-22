cmake_minimum_required(VERSION 3.15..3.20 FATAL_ERROR)
project(CPPFITS VERSION 0.1.0 LANGUAGES CXX)


add_library(fits src/header.cpp)
target_include_directories(fits PUBLIC include)
set_target_properties(fits PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_subdirectory(tests)
