add_library(io_monitor STATIC ioStat.cc ioMap.cc)

target_compile_definitions(io_monitor PUBLIC DEBUG=${DEBUG})

target_include_directories(io_monitor PUBLIC ${CMAKE_SOURCE_DIR}/include)
