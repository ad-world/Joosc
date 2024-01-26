project(joosc)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  # Latest commit from v1.8.x
  URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
)
FetchContent_MakeAvailable(googletest)

# --- Target joosc ----
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*/*.cc") # All files in subdirectories of src
add_executable(joosc)
target_sources(joosc PRIVATE ${SOURCES} "src/main.cc")
target_include_directories(joosc PRIVATE src)

# ---- Target unit_tests ----
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/src/*.cc")
add_executable(unit_tests)
target_sources(unit_tests PRIVATE ${SOURCES} ${TEST_SOURCES})
target_include_directories(unit_tests PRIVATE src tests)
target_link_libraries(unit_tests gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(unit_tests)
