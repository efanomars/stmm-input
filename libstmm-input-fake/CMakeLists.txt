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

# File:   libstmm-input-fake/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(stmm-input-fake CXX)

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
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-input-fake")
# Header files
# depends on
#   libstmm-input
#   libstmm-input-base
#   libstmm-input-ev
set(STMMI_HEADERS_FAKES
        "${STMMI_HEADERS_DIR}/fakedevice.h"
        "${STMMI_HEADERS_DIR}/fakedevicemanager.h"
        "${STMMI_HEADERS_DIR}/fakejoystickdevice.h"
        "${STMMI_HEADERS_DIR}/fakekeydevice.h"
        "${STMMI_HEADERS_DIR}/fakepointerdevice.h"
        "${STMMI_HEADERS_DIR}/faketouchdevice.h"
        "${STMMI_HEADERS_DIR}/stmm-input-fake.h"
        "${STMMI_HEADERS_DIR}/stmm-input-fake-config.h"
        )
# Concatenate header file lists
#set(STMMI_HEADERS)
list(APPEND STMMI_HEADERS
        ${STMMI_HEADERS_FAKES}
        )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
# Concatenate source file lists
list(APPEND  STMMI_SOURCES
        "${STMMI_SOURCES_DIR}/fakedevice.cc"
        "${STMMI_SOURCES_DIR}/fakedevicemanager.cc"
        "${STMMI_SOURCES_DIR}/fakejoystickdevice.cc"
        "${STMMI_SOURCES_DIR}/fakekeydevice.cc"
        "${STMMI_SOURCES_DIR}/fakepointerdevice.cc"
        "${STMMI_SOURCES_DIR}/faketouchdevice.cc"
        "${STMMI_SOURCES_DIR}/stmm-input-fake.cc"
        )

# Define library
add_library(stmm-input-fake ${STMMI_SOURCES} "${PROJECT_BINARY_DIR}/stmm-input-fake-config.cc")

include("stmm-input-fake-defs.cmake")

target_include_directories(stmm-input-fake SYSTEM PUBLIC "${STMMINPUTFAKE_EXTRA_INCLUDE_DIRS}")
target_include_directories(stmm-input-fake BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-fake>  # <prefix>/include/stmm-input-fake
  )

target_include_directories(stmm-input-fake BEFORE PRIVATE ${STMMI_INCLUDE_DIR})

target_link_libraries(stmm-input-fake ${STMMINPUTFAKE_EXTRA_LIBRARIES})

set_target_properties(stmm-input-fake PROPERTIES  ${CMAKE_BUILD_TYPE}_POSTFIX "")
set_target_properties(stmm-input-fake PROPERTIES
        VERSION ${STMM_INPUT_FAKE_VERSION}
        SOVERSION ${STMM_INPUT_FAKE_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-fake-${STMM_INPUT_FAKE_VERSION}"
        )

DefineTargetPublicCompileOptions(stmm-input-fake)

# Set version for stmm-input-fake-config.cc.in
set(STMMI_PKG_VERSION "${STMM_INPUT_FAKE_VERSION}")
# Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-input-fake-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-fake-config.cc" @ONLY)

include(FindPkgConfig)
if (PKG_CONFIG_FOUND)
    # Set install dir for pkg-config file
    set(STMMI_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

    # Create pkg-config file for the library
    configure_file("${PROJECT_SOURCE_DIR}/stmm-input-fake.pc.in"
                   "${PROJECT_BINARY_DIR}/stmm-input-fake.pc" @ONLY)
endif()

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input-fake was configured with the following options:")
message(STATUS " STMMI_HEADERS:                    ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                    ${STMMI_SOURCES}")
message(STATUS " STMMINPUTFAKE_EXTRA_INCLUDE_DIRS: ${STMMINPUTFAKE_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMINPUTFAKE_EXTRA_LIBRARIES:    ${STMMINPUTFAKE_EXTRA_LIBRARIES}")
message(STATUS " CMAKE_BUILD_TYPE:                 ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:            ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:       ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                  ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                   ${CMAKE_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                       ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                    ${BUILD_TESTING}")
message(STATUS " BUILD_SHARED_LIBS:                ${BUILD_SHARED_LIBS}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-input-fake doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

install(TARGETS stmm-input-fake LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS}   DESTINATION "include/stmm-input-fake")

if (PKG_CONFIG_FOUND)
    install(FILES  "${PROJECT_BINARY_DIR}/stmm-input-fake.pc"   DESTINATION "lib/pkgconfig")
endif()
