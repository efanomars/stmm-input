# File: libstmm-input-gtk-dm/CMakeLists.txt

#  Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
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

option(OMIT_PLUGINS "Exclude plugins device manager (stmm-input-dl)" OFF)
mark_as_advanced(OMIT_PLUGINS)

project(stmm-input-gtk-dm CXX)

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
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-input-gtk-dm")
# Header files
# "SubProject" STMMI_HEADERS_DATA depends on
#   libstmm-input
#   gtkmm-3.0-dev
set(STMMI_HEADERS_DATA
        "${STMMI_HEADERS_DIR}/jsdevicefiles.h"
        )
# "SubProject" STMMI_HEADERS_MAS depends on
#   STMMI_HEADERS_DATA
#   libstmm-input-ev
set(STMMI_HEADERS_MAS
# Moved to sources        ${STMMI_HEADERS_DIR}/masgtkdevicemanager.h
        )
# "SubProject" STMMI_HEADERS_JS depends on
#   STMMI_HEADERS_DATA
#   libstmm-input-ev
set(STMMI_HEADERS_JS
# Moved to sources        ${STMMI_HEADERS_DIR}/jsgtkdevicemanager.h
        )
# "SubProject" STMMI_HEADERS_ALL depends on
#   STMMI_HEADERS_MAS
#   STMMI_HEADERS_JS
#   libstmm-input-dl
set(STMMI_HEADERS_ALL
        "${STMMI_HEADERS_DIR}/gtkdevicemanager.h"
        "${STMMI_HEADERS_DIR}/stmm-input-gtk-dm.h"
        "${STMMI_HEADERS_DIR}/stmm-input-gtk-dm-config.h"
        )
# Concatenate header file lists
#set(STMMI_HEADERS)
list(APPEND STMMI_HEADERS
        ${STMMI_HEADERS_DATA}
        ${STMMI_HEADERS_MAS}
        ${STMMI_HEADERS_JS}
        ${STMMI_HEADERS_ALL}
        )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES_DATA
        "${STMMI_SOURCES_DIR}/jsdevicefiles.cc"
        "${STMMI_SOURCES_DIR}/recycler.h"
        )
set(STMMI_SOURCES_MAS
        "${STMMI_SOURCES_DIR}/masgtkbackend.h"
        "${STMMI_SOURCES_DIR}/masgtkbackend.cc"
        "${STMMI_SOURCES_DIR}/masgtkdevicemanager.h"
        "${STMMI_SOURCES_DIR}/masgtkdevicemanager.cc"
        "${STMMI_SOURCES_DIR}/masgtkkeyboarddevice.h"
        "${STMMI_SOURCES_DIR}/masgtkkeyboarddevice.cc"
        "${STMMI_SOURCES_DIR}/masgtklistenerextradata.h"
        "${STMMI_SOURCES_DIR}/masgtkpointerdevice.h"
        "${STMMI_SOURCES_DIR}/masgtkpointerdevice.cc"
        "${STMMI_SOURCES_DIR}/masgtkwindowdata.h"
        "${STMMI_SOURCES_DIR}/masgtkwindowdata.cc"
        )
set(STMMI_SOURCES_JS
        "${STMMI_SOURCES_DIR}/jsgtkbackend.h"
        "${STMMI_SOURCES_DIR}/jsgtkbackend.cc"
        "${STMMI_SOURCES_DIR}/joysticksources.h"
        "${STMMI_SOURCES_DIR}/joysticksources.cc"
        "${STMMI_SOURCES_DIR}/jsgtkdevicemanager.h"
        "${STMMI_SOURCES_DIR}/jsgtkdevicemanager.cc"
        "${STMMI_SOURCES_DIR}/jsgtkjoystickdevice.h"
        "${STMMI_SOURCES_DIR}/jsgtkjoystickdevice.cc"
        "${STMMI_SOURCES_DIR}/jsgtklistenerextradata.h"
        "${STMMI_SOURCES_DIR}/jsgtkwindowdata.h"
        )
set(STMMI_SOURCES_ALL
        "${STMMI_SOURCES_DIR}/gtkdevicemanager.cc"
        )
# Concatenate source file lists
list(APPEND  STMMI_SOURCES
        ${STMMI_SOURCES_DATA}
        ${STMMI_SOURCES_MAS}
        ${STMMI_SOURCES_JS}
        ${STMMI_SOURCES_ALL}
        )

# Define library
add_library(stmm-input-gtk-dm ${STMMI_SOURCES} ${PROJECT_BINARY_DIR}/stmm-input-gtk-dm-config.cc)

include("stmm-input-gtk-dm-defs.cmake")

target_include_directories(stmm-input-gtk-dm SYSTEM PUBLIC ${STMMINPUTGTKDM_EXTRA_INCLUDE_DIRS})
target_include_directories(stmm-input-gtk-dm BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-gtk-dm>  # <prefix>/include/stmm-input-gtk-dm
)

target_link_libraries(stmm-input-gtk-dm ${STMMINPUTGTKDM_EXTRA_LIBRARIES})

set_target_properties(stmm-input-gtk-dm PROPERTIES  ${CMAKE_BUILD_TYPE}_POSTFIX "")
set_target_properties(stmm-input-gtk-dm PROPERTIES
        VERSION ${STMM_INPUT_GTK_DM_VERSION}
        SOVERSION ${STMM_INPUT_GTK_DM_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-gtk-dm-${STMM_INPUT_GTK_DM_VERSION}"
        )

DefineTargetPublicCompileOptions(stmm-input-gtk-dm)

if (OMIT_PLUGINS)
    target_compile_definitions(stmm-input-gtk-dm PUBLIC STMI_OMIT_PLUGINS)
endif()

# Set version for stmm-input-gtk-dm-config.cc.in
set(STMMI_PKG_VERSION "${STMM_INPUT_GTK_DM_VERSION}")
# Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-input-gtk-dm-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-gtk-dm-config.cc" @ONLY)

set(STMMI_GTK_DM_OPT_DL_DEPENDENCY "")
if (NOT OMIT_PLUGINS)
    set(STMMI_GTK_DM_OPT_DL_DEPENDENCY "stmm-input-dl >= ${STMM_INPUT_GTK_DM_REQ_STMM_INPUT_DL_VERSION}")
endif()

# Set install dir for pkg-config file
set(STMMI_GTK_DM_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
# Create pkg-config file for the library
configure_file("${PROJECT_SOURCE_DIR}/stmm-input-gtk-dm.pc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-gtk-dm.pc" @ONLY)

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input-gtk-dm was configured with the following options:")
message(STATUS " STMMI_HEADERS:                     ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                     ${STMMI_SOURCES}")
message(STATUS " STMMINPUTGTKDM_EXTRA_INCLUDE_DIRS: ${STMMINPUTGTKDM_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMINPUTGTKDM_EXTRA_LIBRARIES:    ${STMMINPUTGTKDM_EXTRA_LIBRARIES}")
message(STATUS " CMAKE_BUILD_TYPE:                  ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:             ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:        ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                   ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                    ${STMMI_GTK_DM_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                        ${BUILD_DOCS}")
message(STATUS " ENV{STMM_DEBIAN_PACKAGING}:        $ENV{STMM_DEBIAN_PACKAGING}")
message(STATUS " BUILD_TESTING:                     ${BUILD_TESTING}")
message(STATUS " OMIT_PLUGINS:                      ${OMIT_PLUGINS}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS OR ("$ENV{STMM_DEBIAN_PACKAGING}" STREQUAL "ON"))
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-input-gtk-dm doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

install(TARGETS stmm-input-gtk-dm LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS}   DESTINATION "include/stmm-input-gtk-dm")

install(FILES "${PROJECT_BINARY_DIR}/stmm-input-gtk-dm.pc"   DESTINATION "lib/pkgconfig")
