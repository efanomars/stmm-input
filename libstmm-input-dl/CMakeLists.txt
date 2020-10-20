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

# File:   libstmm-input-dl/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(stmm-input-dl CXX)

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
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-input-dl")
# Header files
set(STMMI_HEADERS
        "${STMMI_HEADERS_DIR}/pluginsdevicemanager.h"
        "${STMMI_HEADERS_DIR}/stmm-input-dl-config.h"
       )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
        "${STMMI_SOURCES_DIR}/dlpinfos.h"
        "${STMMI_SOURCES_DIR}/dlpinfos.cc"
        "${STMMI_SOURCES_DIR}/dlutil.h"
        "${STMMI_SOURCES_DIR}/dlutil.cc"
        "${STMMI_SOURCES_DIR}/plugininterface.cc"
        "${STMMI_SOURCES_DIR}/pluginsdevicemanager.cc"
       )

# Define library
add_library(stmm-input-dl ${STMMI_SOURCES} "${PROJECT_BINARY_DIR}/stmm-input-dl-config.cc")

include("stmm-input-dl-defs.cmake")

target_include_directories(stmm-input-dl SYSTEM PUBLIC "${STMMINPUTDL_EXTRA_INCLUDE_DIRS}")
target_include_directories(stmm-input-dl BEFORE PRIVATE ${STMMI_SOURCES_DIR})
target_include_directories(stmm-input-dl BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-dl>  # <prefix>/include/stmm-input-dl
)

target_link_libraries(stmm-input-dl ${STMMINPUTDL_EXTRA_LIBRARIES})

set_target_properties(stmm-input-dl PROPERTIES  ${CMAKE_BUILD_TYPE}_POSTFIX "")
set_target_properties(stmm-input-dl PROPERTIES
        VERSION ${STMM_INPUT_DL_VERSION}
        SOVERSION ${STMM_INPUT_DL_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-dl-${STMM_INPUT_DL_VERSION}"
        )

DefineTargetPublicCompileOptions(stmm-input-dl)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(stmm-input-dl PUBLIC "-rdynamic")
endif()

# Set version for stmm-input-dl-config.cc.in
set(STMMI_PKG_VERSION "${STMM_INPUT_DL_VERSION}")
# Set plugin directories for stmm-input-dl-config.cc.in
set(STMMI_PKG_DATA_DIR0 "${STMMI_PLUGINS_DATA_DIR0}")
set(STMMI_PKG_DATA_DIR1 "${STMMI_PLUGINS_DATA_DIR1}")
set(STMMI_PKG_USER_DATA_DIR "${STMMI_PLUGINS_USER_DATA_DIR}")
# Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-input-dl-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-dl-config.cc" @ONLY)

include(FindPkgConfig)
if (PKG_CONFIG_FOUND)
    # Set install dir for pkg-config file
    set(STMMI_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

    # Create pkg-config file for the library
    configure_file("${PROJECT_SOURCE_DIR}/stmm-input-dl.pc.in"
                   "${PROJECT_BINARY_DIR}/stmm-input-dl.pc" @ONLY)
endif()

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input-dl was configured with the following options:")
message(STATUS " STMMI_HEADERS:                  ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                  ${STMMI_SOURCES}")
message(STATUS " STMMINPUTDL_EXTRA_INCLUDE_DIRS: ${STMMINPUTDL_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMINPUTDL_EXTRA_LIBRARIES:    ${STMMINPUTDL_EXTRA_LIBRARIES}")
message(STATUS " CMAKE_BUILD_TYPE:               ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:          ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:     ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                 ${CMAKE_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                     ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                  ${BUILD_TESTING}")
message(STATUS " STMMI_PLUGINS_DATA_DIR:         ${STMMI_PLUGINS_DATA_DIR}")
message(STATUS " STMMI_PLUGINS_USER_DATA_DIR:    ${STMMI_PLUGINS_USER_DATA_DIR}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-input-dl doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

install(TARGETS stmm-input-dl LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS} DESTINATION "include/stmm-input-dl")

if (PKG_CONFIG_FOUND)
    install(FILES  "${PROJECT_BINARY_DIR}/stmm-input-dl.pc"   DESTINATION "lib/pkgconfig")
endif()
