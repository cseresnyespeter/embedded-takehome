# CMake minimum version
cmake_minimum_required(VERSION 3.14)

# Project definition
project(ElevatorController C CXX)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set C++ standard for tests
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Main Application ---

# Add all C source files into a library.
# This allows both the main app and the tests to use the same compiled code.
add_library(elevator_lib
    src/seqnet.c
    src/condsel.c
    src/posdet.c
)

# Target the include directory for the library
target_include_directories(elevator_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add the main executable
add_executable(elevator_emulator main.c)

# Link the main executable against the library
target_link_libraries(elevator_emulator PRIVATE elevator_lib)


# --- Google Test Setup ---

# Enable testing
enable_testing()

# Fetch GoogleTest from GitHub. This avoids needing to install it system-wide.
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# Make googletest available
FetchContent_MakeAvailable(googletest)

# --- Unit Test Application ---

# Add the test executable
add_executable(run_tests
    test/test_condsel.cpp
    test/test_seqnet.cpp
    test/mock/mock_posdet.cpp
)

# Include the C headers for the test executable
target_include_directories(run_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/include)
target_include_directories(run_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/test/mock)

# Link the test executable against our library and Google Mock/Test.
target_link_libraries(run_tests PRIVATE elevator_lib gmock_main)

# Add the test to CTest so it can be run automatically
include(GoogleTest)
gtest_discover_tests(run_tests)
