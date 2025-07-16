enable_testing()

# GTest
find_package(GTest REQUIRED)

add_executable(io_monitor_tests test_main.cc test_io_monitor.cc)

target_link_libraries(io_monitor_tests io_monitor GTest::gtest
                      GTest::gtest_main pthread)

add_test(NAME IoMonitorTests COMMAND io_monitor_tests)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip)

FetchContent_MakeAvailable(googletest)
