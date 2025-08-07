add_library(io_monitor STATIC IoStat.cc IoMap.cc IoAggregate.cc
                              IoAggregateMap.cc)

target_include_directories(io_monitor PUBLIC ${CMAKE_SOURCE_DIR}/include)
