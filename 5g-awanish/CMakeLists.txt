# ============================================================
#  5g-awanish : L2/MAC Stack Project
#  Top-level CMake build configuration
# ============================================================

cmake_minimum_required(VERSION 3.14)

project(5g_awanish
    VERSION     0.1.0
    DESCRIPTION "5G L2/MAC Stack — Awanish"
    LANGUAGES   CXX
)

# ---------- C++ Standard ----------
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# ---------- Default Build Type ----------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ---------- Compiler Warnings ----------
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ---------- Output Directories ----------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ---------- Public Header Path ----------
include_directories(${CMAKE_SOURCE_DIR}/include)

# ============================================================
#  Main Executable
# ============================================================
add_executable(5g_awanish
    src/main.cpp
    # Day 3+ source files yahan add honge:
    # src/common/logger.cpp
    # src/common/timer.cpp
    # src/common/config.cpp
    # src/mac/ue_context.cpp
)

# ============================================================
#  Tests (optional — only if gtest is available)
# ============================================================
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ============================================================
#  Status Print
# ============================================================
message(STATUS "----------------------------------------")
message(STATUS "  Project    : ${PROJECT_NAME}")
message(STATUS "  Version    : ${PROJECT_VERSION}")
message(STATUS "  Build Type : ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Std    : ${CMAKE_CXX_STANDARD}")
message(STATUS "  Tests      : ${BUILD_TESTS}")
message(STATUS "----------------------------------------")
