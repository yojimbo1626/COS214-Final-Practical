cmake_minimum_required(VERSION 3.14)
project(MyProject CXX)

# 1. Enforce C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Fetch doctest automatically during the build
include(FetchContent)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest
  GIT_TAG v2.4.11 # Latest stable release
)
FetchContent_MakeAvailable(doctest)

# 3. Enable testing
enable_testing()

# 4. Build your test executable
# (Add these file names manually, also add folders if inside any)

add_executable(file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/*Tests.cpp)
add_executable(unit_tests ${TEST_SOURCES} ...your non-test sources...)
)

# 5. Link doctest (this provides the main() function automatically)
target_link_libraries(unit_tests PRIVATE doctest::doctest)

# 6. Discover individual test cases for CTest
# This requires CMake 3.14+ and is built into doctest
include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")
doctest_discover_tests(unit_tests)