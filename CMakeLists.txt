project(joosc)
cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(FUZZER "Generate a graph of the parse tree" OFF)
if ( NOT FUZZER )
    add_compile_options("-g")
    add_compile_options("-fdiagnostics-color=always")
endif()
if ( FUZZER )
    add_compile_options("-fsanitize=address,undefined")
    add_link_options("-fsanitize=address,undefined")
    # add_compile_options("-fprofile-generate")
    # add_compile_options("-ftest-coverage")
    # add_link_options("-fprofile-generate")
    # add_link_options("-ftest-coverage")
endif()

# Graphing
option(GRAPHVIZ "Generate a graph of the parse tree" OFF)
if(GRAPHVIZ)
    add_definitions("-DGRAPHVIZ")
endif(GRAPHVIZ)
unset(GRAPHVIZ CACHE)

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

# --- Main library ----
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*/*.cc") # All files in subdirectories of src
add_library(CommonLib OBJECT ${SOURCES})
target_include_directories(CommonLib PRIVATE src)

# --- Target joosc ----
add_executable(joosc)
target_sources(joosc PRIVATE $<TARGET_OBJECTS:CommonLib> "src/main.cc")
target_include_directories(joosc PRIVATE src)

# ---- Target unit_tests ----
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/src/*.cc")
add_executable(unit_tests)
target_sources(unit_tests PRIVATE $<TARGET_OBJECTS:CommonLib> ${TEST_SOURCES})
target_include_directories(unit_tests PRIVATE src tests)
target_link_libraries(unit_tests gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(unit_tests)

# ---- Target lex_tester ----
# file(READ "tests/lexing/lex_tester.cpp" LEX_TESTER)
add_executable(lex_tester)
target_sources(lex_tester PRIVATE $<TARGET_OBJECTS:CommonLib> "tests/lexing/lex_tester.cc")
target_include_directories(lex_tester PRIVATE src)

if (FUZZER)
    add_subdirectory(fuzz)
endif (FUZZER)
unset(FUZZER CACHE)
