add_subdirectory(../extern/Catch2 "extern/catch2" EXCLUDE_FROM_ALL)

add_executable(tests
    tests.cpp
    test_header.cpp
    test_file.cpp
)


target_link_libraries(tests PRIVATE Catch2::Catch2 fits)
set_target_properties(tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

include(CTest)
include(../extern/Catch2/contrib/Catch.cmake)
catch_discover_tests(tests)
