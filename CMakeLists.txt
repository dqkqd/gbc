cmake_minimum_required(VERSION 4.0)

project(gb C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIB_SOURCES)

add_executable(gb src/main.c ${LIB_SOURCES})

target_compile_options(
  gb
  PRIVATE -Wall
          -Wextra
          -Wpedantic
          -Wshadow
          -Wundef
          -Wdouble-promotion
          -Wvla
          $<$<CONFIG:Release>:-O2>
          $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
target_link_options(gb PRIVATE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)

target_include_directories(gb PRIVATE include)

# tests
option(GB_TEST "Enable test mode" OFF)
if(GB_TEST)
  target_compile_definitions(gb PRIVATE GB_TEST)
endif()
