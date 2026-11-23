cmake_minimum_required(VERSION 3.20)
project(lab3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(lab3_core STATIC
    src/error/ChecksumMismatchException.cpp
    src/error/EmptyTrashException.cpp
    src/error/EntryAlreadyExistsException.cpp
    src/error/FileLockedException.cpp
    src/error/FileSystemException.cpp
    src/error/InvalidFileNameException.cpp
    src/error/InvalidFileSizeException.cpp
    src/error/NotADirectoryException.cpp
    src/error/NotAFileException.cpp
    src/error/PathNotFoundException.cpp
    src/error/PermissionDeniedException.cpp
    src/error/QuotaExceededException.cpp
    src/error/StorageFullException.cpp
    src/error/VersionConflictException.cpp
    src/value/Checksum.cpp
    src/value/FileExtension.cpp
    src/value/FileSize.cpp
    src/value/Path.cpp
    src/value/Permission.cpp
    src/value/Timestamp.cpp
)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    target_compile_options(lab3_core PUBLIC --coverage -O0 -g)
    target_link_options(lab3_core PUBLIC --coverage)
endif()

target_include_directories(lab3_core PUBLIC include)
