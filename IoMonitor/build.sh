#!/bin/sh
mkdir build && cd build && cmake .. && cd tests && cmake --build .. && ./io_monitor_tests
