# ============================================================
#  Tests — Google Test
# ============================================================

find_package(GTest QUIET)

if(NOT GTest_FOUND)
    message(WARNING "GTest not found — tests will be skipped.")
    message(WARNING "Install with: sudo apt install -y libgtest-dev libgmock-dev")
    return()
endif()

message(STATUS "GTest found — building tests")

# ---------- Sanity Test ----------
add_executable(test_sanity test_sanity.cpp)
target_link_libraries(test_sanity PRIVATE GTest::gtest GTest::gtest_main pthread)
add_test(NAME test_sanity COMMAND test_sanity)

# Day 7 onwards: aur test files yahan add honge
# add_executable(test_logger test_logger.cpp ${CMAKE_SOURCE_DIR}/src/common/logger.cpp)
# target_link_libraries(test_logger PRIVATE GTest::gtest GTest::gtest_main pthread)
# add_test(NAME test_logger COMMAND test_logger)
