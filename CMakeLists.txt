#
# Copyright (c) 2011 LAAS/CNRS
# All rights reserved.
#
# Permission to use, copy, modify, and distribute this software for any purpose
# with or without   fee is hereby granted, provided   that the above  copyright
# notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS  SOFTWARE INCLUDING ALL  IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR  BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR  ANY DAMAGES WHATSOEVER RESULTING  FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION,   ARISING OUT OF OR IN    CONNECTION WITH THE USE   OR
# PERFORMANCE OF THIS SOFTWARE.
#
#                                            François LANCELOT on Fri Sep 16 2011

cmake_minimum_required(VERSION 2.6)

# Configure the project
project(robotModelParser)
set(LIBRARY_OUTPUT_PATH lib)
set(robotModelParserVersion 0.1)

message(STATUS "Compiling robotModelParser Version ${robotModelParserVersion}")

# Add path to find a library to include
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

# Include ColladaDom

FIND_PACKAGE(COLLADA15DOM REQUIRED)
INCLUDE_directories(${COLLADA15DOM_INCLUDE_DIR}/collada-dom)
INCLUDE_directories(${COLLADA15DOM_INCLUDE_DIR}/collada-dom/1.5)
SET(INCLUDE_DIRS ${INCLUDE_DIRS} ${COLLADA15DOM_INCLUDE_DIR}/collada-dom)
SET(INCLUDE_DIRS ${INCLUDE_DIRS} ${COLLADA15DOM_INCLUDE_DIR}/collada-dom/1.5)
SET(LIBS ${LIBS} ${COLLADA15DOM_LIBRARIES})

# Include Boost

#FIND_PACKAGE(BOOST_SYSTEM-MT REQUIRED)
#SET(LIBS ${LIBS} ${BOOST_SYSTEM-MT_LIBRARIES})
#ADD_DEFINITIONS(-pthread)


IF(UNIX)
  ADD_DEFINITIONS(-DUNIX)
  IF(APPLE)
    ADD_DEFINITIONS(-DMACOSX -x c++)
    SET(CMAKE_SHARED_LINKER_FLAGS "-dynamiclib -Wl,-headerpad_max_install_names,-undefined,dynamic_lookup")
  ELSE(APPLE)
    ADD_DEFINITIONS(-DLINUX)
  ENDIF(APPLE)
ELSE(UNIX)
 MESSAGE (FATAL_ERROR "Not supported Platform")
ENDIF(UNIX)

# Default mode is Debug
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

# Source files
file(GLOB_RECURSE source_files "src/*.cpp")
file(GLOB_RECURSE headers "include/*.h")

# To create a Dynamic-Link Library
add_library(
    robotModelParser
    SHARED
    ${source_files}
    ${headers}
)

target_link_libraries(
    robotModelParser
    ${LIBS}
)

# Install target
INSTALL( TARGETS robotModelParser DESTINATION lib PERMISSIONS
OWNER_READ OWNER_WRITE OWNER_EXECUTE
GROUP_READ GROUP_EXECUTE
WORLD_READ WORLD_EXECUTE )

INSTALL(DIRECTORY  ${CMAKE_SOURCE_DIR}/include/
                DESTINATION include/robotModelParser
                FILES_MATCHING REGEX "\\.[h|hpp]")

# Uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
