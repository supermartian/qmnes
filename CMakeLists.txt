# -*-cmake-*-
PROJECT(qmnes)

cmake_minimum_required(VERSION 2.8)

#set(CMAKE_VERBOSE_MAKEFILE true)

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
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -ggdb -O0  -Wall -Wextra -DDEBUG")

SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -g -O0  -Wall -Wextra")

SET(LIBS ${OpenCV_LIBS} ${EXTRA_LIBS})

# Build paths
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

# Include directories
INCLUDE_DIRECTORIES("6502/")
INCLUDE_DIRECTORIES("ppu/")
INCLUDE_DIRECTORIES("apu/")
INCLUDE_DIRECTORIES("rom/")

link_directories("${qmnes_SOURCE_DIR}/6502/")
link_directories("${qmnes_SOURCE_DIR}/ppu/")
link_directories("${qmnes_SOURCE_DIR}/apu/")
link_directories("${qmnes_SOURCE_DIR}/rom/")

add_executable(test
    test.c
    6502/cpu.c
    6502/ins.c
    rom/rom.c)

#cpack stuff goes here

