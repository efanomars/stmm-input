# File: libstmm-input-ev/CMakeLists.txt

#  Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public
#  License along with this program; if not, see <http://www.gnu.org/licenses/>

cmake_minimum_required(VERSION 3.0)

project(stmm-input-ev CXX)

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
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-input-ev")
# Header files
set(STMMI_HEADERS
        "${STMMI_HEADERS_DIR}/devicemgmtcapability.h"
        "${STMMI_HEADERS_DIR}/devicemgmtevent.h"
        "${STMMI_HEADERS_DIR}/joystickcapability.h"
        "${STMMI_HEADERS_DIR}/joystickevent.h"
        "${STMMI_HEADERS_DIR}/keycapability.h"
        "${STMMI_HEADERS_DIR}/keyevent.h"
        "${STMMI_HEADERS_DIR}/pointercapability.h"
        "${STMMI_HEADERS_DIR}/pointerevent.h"
        "${STMMI_HEADERS_DIR}/stddevicemanager.h"
        "${STMMI_HEADERS_DIR}/stmm-input-ev.h"
        "${STMMI_HEADERS_DIR}/stmm-input-ev-config.h"
        "${STMMI_HEADERS_DIR}/touchcapability.h"
        "${STMMI_HEADERS_DIR}/touchevent.h"
       )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
        "${STMMI_SOURCES_DIR}/devicemgmtcapability.cc"
        "${STMMI_SOURCES_DIR}/devicemgmtevent.cc"
        "${STMMI_SOURCES_DIR}/joystickcapability.cc"
        "${STMMI_SOURCES_DIR}/joystickevent.cc"
        "${STMMI_SOURCES_DIR}/keycapability.cc"
        "${STMMI_SOURCES_DIR}/keyevent.cc"
        "${STMMI_SOURCES_DIR}/pointercapability.cc"
        "${STMMI_SOURCES_DIR}/pointerevent.cc"
        "${STMMI_SOURCES_DIR}/stddevicemanager.cc"
        "${STMMI_SOURCES_DIR}/touchcapability.cc"
        "${STMMI_SOURCES_DIR}/touchevent.cc"
        "${STMMI_SOURCES_DIR}/utilev.h"
       )

# Define library
add_library(stmm-input-ev ${STMMI_SOURCES} "${PROJECT_BINARY_DIR}/stmm-input-ev-config.cc")

include("stmm-input-ev-defs.cmake")

target_include_directories(stmm-input-ev SYSTEM PUBLIC ${STMMINPUTEV_EXTRA_INCLUDE_DIRS})
target_include_directories(stmm-input-ev BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-ev>  # <prefix>/include/stmm-input-ev
  )

#target_link_libraries(stmm-input-ev stmm-input-base)
target_link_libraries(stmm-input-ev ${STMMINPUTEV_EXTRA_LIBRARIES})

set_target_properties(stmm-input-ev PROPERTIES  ${CMAKE_BUILD_TYPE}_POSTFIX "")
set_target_properties(stmm-input-ev PROPERTIES
        VERSION ${STMM_INPUT_EV_VERSION}
        SOVERSION ${STMM_INPUT_EV_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-ev-${STMM_INPUT_EV_VERSION}"
        )

DefineTargetPublicCompileOptions(stmm-input-ev)

# Set version for stmm-input-ev-config.cc.in
set(STMMI_PKG_VERSION "${STMM_INPUT_EV_VERSION}")
# Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-input-ev-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-ev-config.cc" @ONLY)

include(FindPkgConfig)
if (PKG_CONFIG_FOUND)
    # Set install dir for pkg-config file
    set(STMMI_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

    # Create pkg-config file for the library
    configure_file("${PROJECT_SOURCE_DIR}/stmm-input-ev.pc.in"
                   "${PROJECT_BINARY_DIR}/stmm-input-ev.pc" @ONLY)
endif()

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input-ev was configured with the following options:")
message(STATUS " STMMI_HEADERS:                  ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                  ${STMMI_SOURCES}")
message(STATUS " STMMINPUTEV_EXTRA_INCLUDE_DIRS: ${STMMINPUTEV_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMINPUTEV_EXTRA_LIBRARIES:    ${STMMINPUTEV_EXTRA_LIBRARIES}")
message(STATUS " CMAKE_BUILD_TYPE:               ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:          ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:     ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                 ${CMAKE_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                     ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                  ${BUILD_TESTING}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-input-ev doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

install(TARGETS stmm-input-ev LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS} DESTINATION "include/stmm-input-ev")

if (PKG_CONFIG_FOUND)
    install(FILES  "${PROJECT_BINARY_DIR}/stmm-input-ev.pc"   DESTINATION "lib/pkgconfig")
endif()
