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
    src/operations/CopyOperation.cpp
    src/operations/CreateOperation.cpp
    src/operations/DeleteOperation.cpp
    src/operations/MoveOperation.cpp
    src/operations/Operation.cpp
    src/operations/OperationJournal.cpp
    src/operations/RenameOperation.cpp
    src/search/CompositeFilter.cpp
    src/search/DateFilter.cpp
    src/search/ExtensionFilter.cpp
    src/search/NameFilter.cpp
    src/search/SearchService.cpp
    src/search/SizeFilter.cpp
    src/search/TagFilter.cpp
    src/service/DirectoryListing.cpp
    src/service/FileInfo.cpp
    src/service/FileManager.cpp
    src/service/ReportService.cpp
    src/service/SearchQuery.cpp
    src/service/ShareLink.cpp
    src/service/ShareService.cpp
    src/service/StorageStats.cpp
    src/service/ReportService.cpp
    src/metadata/EntryVersion.cpp
    src/metadata/FileMetadata.cpp
    src/metadata/Tag.cpp
    src/metadata/VersionHistory.cpp
    src/storage/BackupService.cpp
    src/storage/Quota.cpp
    src/storage/QuotaManager.cpp
    src/storage/Storage.cpp
    src/storage/TrashBin.cpp
    src/user/AccessControlList.cpp
    src/user/Administrator.cpp
    src/user/Guest.cpp
    src/user/Group.cpp
    src/user/GroupRegistry.cpp
    src/user/Lock.cpp
    src/user/LockManager.cpp
    src/user/UserRegistry.cpp
    src/user/RegularUser.cpp
    src/user/User.cpp
    src/entry/ArchiveFile.cpp
    src/entry/BinaryFile.cpp
    src/entry/Directory.cpp
    src/entry/File.cpp
    src/entry/FileSystemEntry.cpp
    src/entry/SymbolicLink.cpp
    src/entry/TextFile.cpp
    src/value/Checksum.cpp
    src/value/Clock.cpp
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

add_executable(lab3_app
    app/ConsoleMenu.cpp
    app/main.cpp
)
target_include_directories(lab3_app PRIVATE app)
target_link_libraries(lab3_app PRIVATE lab3_core)

include(FetchContent)
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

add_executable(lab3_tests
    tests/entry_test.cpp
    tests/error_test.cpp
    tests/metadata_test.cpp
    tests/operations_test.cpp
    tests/search_test.cpp
    tests/service_test.cpp
    tests/storage_test.cpp
    tests/user_test.cpp
    tests/value_test.cpp
)
target_link_libraries(lab3_tests PRIVATE lab3_core GTest::gtest_main)
gtest_discover_tests(lab3_tests)
