cmake_minimum_required(VERSION 3.16)
project(BinarySearchTreeLab CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

add_executable(bst_app
    main.cpp
)
target_include_directories(bst_app PRIVATE ${CMAKE_SOURCE_DIR})

add_executable(run_bst_tests
    tests/gtest_main.cpp
    tests/bst_tests.cpp
)
target_include_directories(run_bst_tests PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(run_bst_tests PRIVATE GTest::gtest GTest::gtest_main)

add_test(NAME run_all_bst_tests COMMAND run_bst_tests)