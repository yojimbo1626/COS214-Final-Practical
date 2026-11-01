cmake_minimum_required(VERSION 3.14)
project(TIVIDY CXX)

# 1. Enforce C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 2. Fetch third-party code automatically during configure
include(FetchContent)
FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest
  GIT_TAG v2.4.11
)
FetchContent_MakeAvailable(doctest)

# cpp-httplib: single-header HTTP server for the GUI (MIT licence)
set(HTTPLIB_USE_OPENSSL_IF_AVAILABLE OFF CACHE BOOL "" FORCE)
set(HTTPLIB_USE_ZLIB_IF_AVAILABLE OFF CACHE BOOL "" FORCE)
set(HTTPLIB_USE_BROTLI_IF_AVAILABLE OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib
  GIT_TAG v0.14.3
)
FetchContent_MakeAvailable(httplib)

find_package(Threads REQUIRED)

# 3. Enable testing
enable_testing()

# 4. The engine: every class under src/ (shared by the tests and the GUI)
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_library(tividy_engine STATIC ${ENGINE_SOURCES})
target_include_directories(tividy_engine PUBLIC ${CMAKE_SOURCE_DIR}/include)

# 5. The application layer: one workflow instance per WorkflowSession, and the GUI server
add_library(tividy_app STATIC ${CMAKE_SOURCE_DIR}/app/WorkflowSession.cpp)
target_include_directories(tividy_app PUBLIC ${CMAKE_SOURCE_DIR}/app)
target_link_libraries(tividy_app PUBLIC tividy_engine)

add_executable(tividy_gui ${CMAKE_SOURCE_DIR}/app/gui_main.cpp)
target_link_libraries(tividy_gui PRIVATE tividy_app httplib::httplib Threads::Threads)

# 6. Unit and integration tests (main() comes from tests/PlaceHolderTests.cpp)
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/tests/*.cpp)
add_executable(unit_tests ${TEST_SOURCES})
target_include_directories(unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(unit_tests PRIVATE tividy_engine doctest::doctest)

# 7. Register each doctest test case with CTest
include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")
doctest_discover_tests(unit_tests)
