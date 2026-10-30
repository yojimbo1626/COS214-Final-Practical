cmake_minimum_required(VERSION 3.14)
project(MyProject CXX)

# 1. Enforce C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Fetch doctest automatically during configure
include(FetchContent)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest
  GIT_TAG v2.4.11
)
FetchContent_MakeAvailable(doctest)

# 3. Enable testing
enable_testing()

# 4. Collect sources automatically
# Tests go anywhere under tests/, class code under src/
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tests/*.cpp)
file(GLOB_RECURSE APP_SOURCES  CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(unit_tests ${APP_SOURCES} ${TEST_SOURCES})

target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 5. Link doctest (header and include path; main() comes from one test file)
target_link_libraries(unit_tests PRIVATE doctest::doctest)

# 6. Register each doctest test case with CTest
include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")
doctest_discover_tests(unit_tests)