cmake_minimum_required(VERSION 3.20)
project(lab1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

add_library(lab1_core STATIC
    src/point.cpp
    src/rectangle.cpp
    src/set_element.cpp
    src/set.cpp
    src/set_parser.cpp
)
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    target_compile_options(lab1_core PUBLIC --coverage -O0 -g)
    target_link_options(lab1_core PUBLIC --coverage)
endif()

target_include_directories(lab1_core PUBLIC include)

add_executable(lab1_app app/main.cpp app/menu.cpp)
target_include_directories(lab1_app PRIVATE app)
target_link_libraries(lab1_app PRIVATE lab1_core)

add_executable(lab1_tests
    tests/point_test.cpp
    tests/rectangle_test.cpp
    tests/set_test.cpp
    tests/parser_test.cpp
)
target_link_libraries(lab1_tests PRIVATE lab1_core GTest::gtest_main)
gtest_discover_tests(lab1_tests)

