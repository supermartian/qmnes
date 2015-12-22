# -*-cmake-*-
PROJECT(qmnes)

cmake_minimum_required(VERSION 2.8)

#set(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${qmnes_SOURCE_DIR}/cmake")
find_library(AALIB_LIBRARY aa)

IF(APPLE)
find_library(FOUNDATION Foundation)
SET(EXTRA_LIBS ${FOUNDATION})
ENDIF()

# C++ Preprocessor definitions
ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE)
ADD_DEFINITIONS(-D_WITH_AVATAR_)

# Compiling and linking
# - type
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
if(DEFINED CMAKE_BUILD_TYPE AND CMAKE_VERSION VERSION_GREATER "2.8")
   set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})
endif()
SET(CMAKE_BUILD_TYPE Release CACHE STRING "")

# - flags
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -O0  -Wall -Wextra -DDEBUG")

SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -g -O0  -Wall -Wextra")

# Build paths
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(CMAKE_CXX_FLAGS "-O0 -g")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

# Include directories
INCLUDE_DIRECTORIES("6502/")
INCLUDE_DIRECTORIES("ppu/")
INCLUDE_DIRECTORIES("apu/")
INCLUDE_DIRECTORIES("rom/")


link_directories("${qmnes_SOURCE_DIR}/6502/")
link_directories("${qmnes_SOURCE_DIR}/ppu/")
link_directories("${qmnes_SOURCE_DIR}/apu/")
link_directories("${qmnes_SOURCE_DIR}/rom/")

add_executable(qmnes
    qmnes.c
    6502/cpu.c
    6502/ins.c
    ppu/ppu.c
    rom/rom.c)

add_executable(qmnes-aa
    qmnes-aa.c
    6502/cpu.c
    6502/ins.c
    ppu/ppu.c
    rom/rom.c)

add_executable(test
    test.c
    6502/cpu.c
    6502/ins.c
    ppu/ppu.c
    rom/rom.c)
message(STATUS "${CMAKE_CXX_FLAGS}")
target_link_libraries(qmnes ${SDL2_LIBRARY})
target_link_libraries(qmnes-aa ${AALIB_LIBRARY})

#cpack stuff goes here

