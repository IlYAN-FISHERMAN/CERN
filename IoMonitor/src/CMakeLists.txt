add_library(io_monitor STATIC io_monitor.cc)

target_include_directories(io_monitor PUBLIC ${CMAKE_SOURCE_DIR}/include)
