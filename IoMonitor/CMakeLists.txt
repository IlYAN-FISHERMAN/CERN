cmake_minimum_required(VERSION 3.16)
project(IoMonitor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Werror -Wpedantic -O2)
endif()

# Include subdirectories
include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)
