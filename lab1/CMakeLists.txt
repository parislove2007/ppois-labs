cmake_minimum_required(VERSION 3.20)
project(lab1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(lab1_core STATIC src/point.cpp src/rectangle.cpp)
target_include_directories(lab1_core PUBLIC include)
add_executable(lab1_app app/main.cpp)
target_link_libraries(lab1_app PRIVATE lab1_core)

