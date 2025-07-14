cmake_minimum_required(VERSION 3.16...3.30 FATAL_ERROR)

project(
  IoStat
  DESCRIPTION "IoStat class for EOS"
  LANGUAGES CXX)

set(WORK_DIR "/srcs/IoStat_lib/")

add_executable(iostat ${CMAKE_HOME_DIRECTORY}${WORK_DIR}/IoStat.cc
                      ${CMAKE_HOME_DIRECTORY}/srcs/main.cc)

target_compile_options(iostat PRIVATE -Wall -Werror -Wextra)
