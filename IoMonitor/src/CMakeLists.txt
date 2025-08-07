add_library(io_monitor STATIC ioStat.cc ioMap.cc ioAggregate.cc
                              ioAggregateMap.cc)

target_include_directories(io_monitor PUBLIC ${CMAKE_SOURCE_DIR}/include)
