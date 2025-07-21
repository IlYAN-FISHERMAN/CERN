cmake_minimum_required(VERSION 3.16...3.30 FATAL_ERROR)

project(
  IoMonitor
  DESCRIPTION "IoMonitor class for logfile"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Werror -Wpedantic -O2 -g3)
endif()

include_directories(include)
add_subdirectory(src)
add_subdirectory(tests)
