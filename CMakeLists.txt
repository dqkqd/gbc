cmake_minimum_required(VERSION 3.31)

project(gb C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIB_SOURCES src/mem.c src/cpu.c src/emu.c src/tile.c src/ppu/sdl.c
                src/ppu.c)

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

include(FetchContent)

set(SDL_SHARED OFF)
set(SDL_STATIC ON)
set(SDL_AUDIO OFF)
set(SDL_CAMERA OFF)
set(SDL_JOYSTICK OFF)
set(SDL_HAPTIC OFF)
set(SDL_SENSOR OFF)
set(SDL_DIALOG OFF)
set(SDL_TRAY OFF)
set(SDL_WAYLAND_SHARED OFF)
set(SDL_WAYLAND_LIBDECOR_SHARED OFF)
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.4.18)
FetchContent_MakeAvailable(SDL3)
target_link_libraries(gb PRIVATE SDL3::SDL3-static)

# tests
option(GB_TEST "Enable test mode" OFF)
if(GB_TEST)
  target_compile_definitions(gb PRIVATE GB_TEST)
endif()
