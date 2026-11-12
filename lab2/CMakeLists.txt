cmake_minimum_required(VERSION 3.20)
project(lab2 LANGUAGES CXX)

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

add_library(lab2_core STATIC
    src/error/BusinessRuleException.cpp
    src/error/ClientNotFoundException.cpp
    src/error/EstimateNotApprovedException.cpp
    src/error/InvalidMoneyAmountException.cpp
    src/error/InvalidPhoneNumberException.cpp
    src/error/InvalidSerialNumberException.cpp
    src/error/InvalidStatusTransitionException.cpp
    src/error/NotFoundException.cpp
    src/error/OrderNotFoundException.cpp
    src/error/OutOfStockException.cpp
    src/error/PartNotFoundException.cpp
    src/error/RepairException.cpp
    src/error/ValidationException.cpp
    src/error/WarrantyExpiredException.cpp
)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    target_compile_options(lab2_core PUBLIC --coverage -O0 -g)
    target_link_options(lab2_core PUBLIC --coverage)
endif()

target_include_directories(lab2_core PUBLIC include)

add_executable(lab2_tests
    tests/error_test.cpp
)
target_link_libraries(lab2_tests PRIVATE lab2_core GTest::gtest_main)
gtest_discover_tests(lab2_tests)
