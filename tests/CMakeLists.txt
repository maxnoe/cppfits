add_subdirectory(../extern/Catch2 "extern/catch2" EXCLUDE_FROM_ALL)

add_executable(tests tests.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2 fits)

include(CTest)
list(APPEND CMAKE_MODULE_PATH ../extern/Catch2/contrib)
include(ParseAndAddCatchTests)
ParseAndAddCatchTests(tests)
