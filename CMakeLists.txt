cmake_minimum_required(VERSION 3.15..3.20 FATAL_ERROR)
project(CPPFITS VERSION 0.1.0 LANGUAGES CXX)

# 1.74 needed for flaoting point endianness
find_package(Boost 1.74.0 REQUIRED)
find_package(xtensor REQUIRED)


add_library(fits src/header.cpp src/file.cpp src/hdu.cpp)
target_include_directories(fits PUBLIC include)
set_target_properties(fits PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_link_libraries(fits PUBLIC xtensor Boost::boost)

add_subdirectory(tests)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE fits)
set_target_properties(main PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
