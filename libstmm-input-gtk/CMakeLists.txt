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

# File:   libstmm-input-gtk/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(stmm-input-gtk CXX)

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
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-input-gtk")
# Header files
# "SubProject" STMMI_HEADERS_DATA depends on
#   libstmm-input
#   gtkmm-3.0-dev
set(STMMI_HEADERS
        "${STMMI_HEADERS_DIR}/gdkkeyconverter.h"
        "${STMMI_HEADERS_DIR}/gdkkeyconverterevdev.h"
        "${STMMI_HEADERS_DIR}/gtkaccessor.h"
        "${STMMI_HEADERS_DIR}/keyrepeatmode.h"
        "${STMMI_HEADERS_DIR}/stmm-input-gtk-config.h"
        "${STMMI_HEADERS_DIR}/stmm-input-gtk.h"
        )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
        "${STMMI_SOURCES_DIR}/gdkkeyconverter.cc"
        "${STMMI_SOURCES_DIR}/gdkkeyconverterevdev.cc"
        "${STMMI_SOURCES_DIR}/gtkaccessor.cc"
        "${STMMI_SOURCES_DIR}/keyrepeatmode.cc"
        "${STMMI_SOURCES_DIR}/stmm-input-gtk.cc"
        )

# Define library
add_library(stmm-input-gtk ${STMMI_SOURCES} "${PROJECT_BINARY_DIR}/stmm-input-gtk-config.cc")

include("stmm-input-gtk-defs.cmake")

target_include_directories(stmm-input-gtk SYSTEM PUBLIC "${STMMINPUTGTK_EXTRA_INCLUDE_DIRS}")
target_include_directories(stmm-input-gtk BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-gtk>  # <prefix>/include/stmm-input-gtk
)

#target_link_libraries(stmm-input-gtk stmm-input)
target_link_libraries(stmm-input-gtk ${STMMINPUTGTK_EXTRA_LIBRARIES})

set_target_properties(stmm-input-gtk PROPERTIES  ${CMAKE_BUILD_TYPE}_POSTFIX "")
set_target_properties(stmm-input-gtk PROPERTIES
        VERSION ${STMM_INPUT_GTK_VERSION}
        SOVERSION ${STMM_INPUT_GTK_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-gtk-${STMM_INPUT_GTK_VERSION}"
        )

DefineTargetPublicCompileOptions(stmm-input-gtk)

# Set version for stmm-input-gtk-config.cc.in
set(STMMI_PKG_VERSION "${STMM_INPUT_GTK_VERSION}")
# Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-input-gtk-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-gtk-config.cc" @ONLY)

# Set install dir for pkg-config file
set(STMMI_GTK_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
# Create pkg-config file for the library
configure_file("${PROJECT_SOURCE_DIR}/stmm-input-gtk.pc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-gtk.pc" @ONLY)

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input-gtk was configured with the following options:")
message(STATUS " STMMI_HEADERS:                   ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                   ${STMMI_SOURCES}")
message(STATUS " STMMINPUTGTK_EXTRA_INCLUDE_DIRS: ${STMMINPUTGTK_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMINPUTGTK_EXTRA_LIBRARIES:    ${STMMINPUTGTK_EXTRA_LIBRARIES}")
message(STATUS " BUILD_SHARED:                    ${BUILD_SHARED}")
message(STATUS " CMAKE_BUILD_TYPE:                ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:           ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:      ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                 ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                  ${STMMI_GTK_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                      ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                   ${BUILD_TESTING}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-input-gtk doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

install(TARGETS stmm-input-gtk LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS}   DESTINATION "include/stmm-input-gtk")

install(FILES  "${PROJECT_BINARY_DIR}/stmm-input-gtk.pc"   DESTINATION "lib/pkgconfig")
