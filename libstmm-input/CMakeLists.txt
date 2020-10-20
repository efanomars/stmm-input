# Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public
# License along with this program; if not, see <http://www.gnu.org/licenses/>

# File:   libstmm-input/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(stmm-input CXX)

set(LIBRARY_OUTPUT_DIRECTORY "build")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../share/cmake")

include(CommonUtil)

CheckBinaryNotSourceTree()
CheckBuildType()
DefineSharedLibOption()
DefineCommonOptions()
DefineCommonCompileOptions("c++14")

# Headers dir
set(STMMI_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/include")
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-input")
# Header files
set(STMMI_HEADERS
        "${STMMI_HEADERS_DIR}/accessor.h"
        "${STMMI_HEADERS_DIR}/callifs.h"
        "${STMMI_HEADERS_DIR}/capability.h"
        "${STMMI_HEADERS_DIR}/device.h"
        "${STMMI_HEADERS_DIR}/devicemanager.h"
        "${STMMI_HEADERS_DIR}/event.h"
        "${STMMI_HEADERS_DIR}/hardwarekey.h"
        "${STMMI_HEADERS_DIR}/private-namedtypes.h"
        "${STMMI_HEADERS_DIR}/stmm-input.h"
        "${STMMI_HEADERS_DIR}/stmm-input-config.h"
        "${STMMI_HEADERS_DIR}/xyevent.h"
        )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
        "${STMMI_SOURCES_DIR}/accessor.cc"
        "${STMMI_SOURCES_DIR}/callifs.cc"
        "${STMMI_SOURCES_DIR}/capability.cc"
        "${STMMI_SOURCES_DIR}/device.cc"
        "${STMMI_SOURCES_DIR}/devicemanager.cc"
        "${STMMI_SOURCES_DIR}/event.cc"
        "${STMMI_SOURCES_DIR}/hardwarekey.cc"
        "${STMMI_SOURCES_DIR}/private-namedtypes.cc"
        "${STMMI_SOURCES_DIR}/stmm-input.cc"
        "${STMMI_SOURCES_DIR}/xyevent.cc"
        )

# Define library
add_library(stmm-input ${STMMI_SOURCES} "${PROJECT_BINARY_DIR}/stmm-input-config.cc")

include("stmm-input-defs.cmake")

target_include_directories(stmm-input SYSTEM PUBLIC ${STMMINPUT_EXTRA_INCLUDE_DIRS})
target_include_directories(stmm-input BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input>  # <prefix>/include/stmm-input
)

target_link_libraries(stmm-input ${STMMINPUT_EXTRA_LIBRARIES})

set_target_properties(stmm-input PROPERTIES  ${CMAKE_BUILD_TYPE}_POSTFIX "")
set_target_properties(stmm-input PROPERTIES
        VERSION ${STMM_INPUT_VERSION}
        SOVERSION ${STMM_INPUT_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-${STMM_INPUT_VERSION}"
        )

DefineTargetPublicCompileOptions(stmm-input)

# Set version for stmm-input-config.cc.in
set(STMMI_PKG_VERSION "${STMM_INPUT_VERSION}")
# Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-input-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-config.cc" @ONLY)

include(FindPkgConfig)
if (PKG_CONFIG_FOUND)
    # Set install dir for pkg-config file
    set(STMMI_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

    # Create pkg-config file for the library
    configure_file("${PROJECT_SOURCE_DIR}/stmm-input.pc.in"
                   "${PROJECT_BINARY_DIR}/stmm-input.pc" @ONLY)
endif()

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input was configured with the following options:")
message(STATUS " STMMI_HEADERS:                ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                ${STMMI_SOURCES}")
message(STATUS " STMMINPUT_EXTRA_INCLUDE_DIRS: ${STMMINPUT_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMINPUT_EXTRA_LIBRARIES:    ${STMMINPUT_EXTRA_LIBRARIES}")
message(STATUS " CMAKE_BUILD_TYPE:             ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:        ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:   ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:              ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:               ${CMAKE_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                   ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                ${BUILD_TESTING}")
message(STATUS " BUILD_WITH_SANITIZE:          ${BUILD_WITH_SANITIZE}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-input doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

install(TARGETS stmm-input LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS} DESTINATION "include/stmm-input")

if (PKG_CONFIG_FOUND)
    install(FILES  "${PROJECT_BINARY_DIR}/stmm-input.pc"   DESTINATION "lib/pkgconfig")
endif()

