enable_testing()

# GTest
find_package(GTest REQUIRED)

add_executable(io_monitor_tests testMain.cc testIoStat.cc testIoMap.cc)

target_compile_definitions(io_monitor_tests PRIVATE DEBUG=${DEBUG})

target_link_libraries(io_monitor_tests io_monitor GTest::gtest
                      GTest::gtest_main pthread)

add_test(NAME IoMonitorTests COMMAND io_monitor_tests)

include(FetchContent)
