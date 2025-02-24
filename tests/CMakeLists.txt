cmake_minimum_required(VERSION 3.14)
project(tests C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_C_STANDARD 99)

include(FetchContent)

FetchContent_Declare(
    sdl3
    URL https://github.com/libsdl-org/SDL/releases/download/release-3.2.4/SDL3-devel-3.2.4-VC.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)

FetchContent_MakeAvailable(sdl3)
message(STATUS "Extracted files are located in: ${sdl3_SOURCE_DIR}")

set(PICO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")

set(base_sources
  ${PICO_DIR}/api.c
  ${PICO_DIR}/z8lua/lapi.cpp
  ${PICO_DIR}/z8lua/lauxlib.cpp
  ${PICO_DIR}/z8lua/lbaselib.cpp
  ${PICO_DIR}/z8lua/lbitlib.cpp
  ${PICO_DIR}/z8lua/lcode.cpp
  ${PICO_DIR}/z8lua/lcorolib.cpp
  ${PICO_DIR}/z8lua/lctype.cpp
  ${PICO_DIR}/z8lua/ldblib.cpp
  ${PICO_DIR}/z8lua/ldebug.cpp
  ${PICO_DIR}/z8lua/ldo.cpp
  ${PICO_DIR}/z8lua/ldump.cpp
  ${PICO_DIR}/z8lua/lfunc.cpp
  ${PICO_DIR}/z8lua/lgc.cpp
  ${PICO_DIR}/z8lua/linit.cpp
  ${PICO_DIR}/z8lua/liolib.cpp
  ${PICO_DIR}/z8lua/llex.cpp
  ${PICO_DIR}/z8lua/lmem.cpp
  ${PICO_DIR}/z8lua/loadlib.cpp
  ${PICO_DIR}/z8lua/lobject.cpp
  ${PICO_DIR}/z8lua/lopcodes.cpp
  ${PICO_DIR}/z8lua/loslib.cpp
  ${PICO_DIR}/z8lua/lparser.cpp
  ${PICO_DIR}/z8lua/lpico8lib.cpp
  ${PICO_DIR}/z8lua/lstate.cpp
  ${PICO_DIR}/z8lua/lstring.cpp
  ${PICO_DIR}/z8lua/lstrlib.cpp
  ${PICO_DIR}/z8lua/ltable.cpp
  ${PICO_DIR}/z8lua/ltablib.cpp
  ${PICO_DIR}/z8lua/ltests.cpp
  ${PICO_DIR}/z8lua/ltm.cpp
  ${PICO_DIR}/z8lua/lundump.cpp
  ${PICO_DIR}/z8lua/lvm.cpp
  ${PICO_DIR}/z8lua/lzio.cpp)

set(tests_sources ${CMAKE_CURRENT_SOURCE_DIR}/tests.c)
set(graphics_sources ${CMAKE_CURRENT_SOURCE_DIR}/graphics.c)

add_library(SDL3 STATIC IMPORTED)

set_target_properties(SDL3 PROPERTIES
  IMPORTED_LOCATION "${sdl3_SOURCE_DIR}/lib/x86/SDL3.lib")

add_executable(tests ${base_sources} ${tests_sources})
target_link_libraries(tests PRIVATE SDL3)

add_executable(graphics WIN32 ${base_sources} ${graphics_sources})
target_link_libraries(graphics PRIVATE SDL3)

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

configure_file("${sdl3_SOURCE_DIR}/lib/x86/SDL3.dll" ${CMAKE_CURRENT_SOURCE_DIR} COPYONLY)

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/../src"
  "${sdl3_SOURCE_DIR}/include")
