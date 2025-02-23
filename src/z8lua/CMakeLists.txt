# Do not modify this file unless absolutely necessary.
# All project-specific settings can be found in the file:
# project_config.cmake

cmake_minimum_required(VERSION 3.10)

if(DEFINED ENV{NGAGESDK})
  set(NGAGESDK $ENV{NGAGESDK})
  set(CMAKE_TOOLCHAIN_FILE ${NGAGESDK}/cmake/ngage-toolchain.gcc29.cmake)
else()
  message(FATAL_ERROR "The environment variable NGAGESDK needs to be defined.")
endif()

project(z8lua C CXX)

set(GCC_MODULE_COMN_DEFS -D__SYMBIAN32__ -D__GCC32__ -D__EPOC32__ -D__MARM__ -D__MARM_ARMI__)
set(GCC_MODULE_MODE_DEFS -DNDEBUG -D_UNICODE)
set(GCC_MODULE_DEFS      ${GCC_MODULE_COMN_DEFS} ${GCC_MODULE_MODE_DEFS})

set(z8lua_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/lapi.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lauxlib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lbaselib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lbitlib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lcode.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lcorolib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lctype.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ldblib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ldebug.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ldo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ldump.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lfunc.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lgc.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/linit.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/liolib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/llex.cpp
#  ${CMAKE_CURRENT_SOURCE_DIR}/lmathlib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lmem.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/loadlib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lobject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lopcodes.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/loslib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lparser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lpico8lib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lstate.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lstring.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lstrlib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ltable.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ltablib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ltests.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ltm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lundump.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lvm.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/lzio.cpp)

add_library(z8lua STATIC ${z8lua_sources})

target_compile_definitions(
  z8lua
  PUBLIC
  ${GCC_MODULE_DEFS})

target_compile_options(
  z8lua
  PUBLIC
  -O3)
