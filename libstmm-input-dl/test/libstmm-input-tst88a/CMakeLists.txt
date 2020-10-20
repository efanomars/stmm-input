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

# File:   libstmm-input-dl/test/libstmm-input-tst88a/CMakeLists.txt

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_TST88A_MAJOR_VERSION 0)
set(STMM_INPUT_TST88A_MINOR_VERSION 1)
set(STMM_INPUT_TST88A_VERSION "${STMM_INPUT_TST88A_MAJOR_VERSION}.${STMM_INPUT_TST88A_MINOR_VERSION}.0")
#
# Headers dir
set(STMMI_TST88A_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/test/libstmm-input-tst88a/include")
set(STMMI_TST88A_HEADERS_DIR  "${STMMI_TST88A_INCLUDE_DIR}/stmm-input-tst88a")
# Header files
set(STMMI_TST88A_HEADERS
        "${STMMI_TST88A_HEADERS_DIR}/tst88capability.h"
      )
#
# Sources dir
set(STMMI_TST88A_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test/libstmm-input-tst88a/src")
# Source files (and headers only used for building)
set(STMMI_TST88A_SOURCES
        "${STMMI_TST88A_SOURCES_DIR}/tst88capability.cc"
      )

# Define library
add_library(stmm-input-tst88a ${STMMI_TST88A_SOURCES}) # SHARED

target_include_directories(stmm-input-tst88a SYSTEM PUBLIC "${STMMI_TST88A_EXTRA_INCLUDE_DIRS}")
target_include_directories(stmm-input-tst88a BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_TST88A_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-tst88a>  # <prefix>/include/stmm-input-tst88a
)

#target_link_libraries(stmm-input-tst88a "${STMMI_TST88A_EXTRA_LIBS}")
target_link_libraries(stmm-input-tst88a stmm-input)

set_target_properties(stmm-input-tst88a   PROPERTIES
        VERSION ${STMM_INPUT_TST88A_VERSION}
        SOVERSION ${STMM_INPUT_TST88A_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-tst88a-${STMM_INPUT_TST88A_VERSION}"
        )
set_target_properties(stmm-input-tst88a   PROPERTIES
        DEBUG_POSTFIX ""
        RELEASE_POSTFIX ""
        )

DefineTargetPublicCompileOptions(stmm-input-tst88a)

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input-tst88a was configured with the following options:")
message(STATUS " STMMI_TST88A_HEADERS:            ${STMMI_TST88A_HEADERS}")
message(STATUS " STMMI_TST88A_SOURCES:            ${STMMI_TST88A_SOURCES}")
message(STATUS " STMMI_TST88A_EXTRA_INCLUDE_DIRS: ${STMMI_TST88A_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMI_TST88A_EXTRA_LIBS:         ${STMMI_TST88A_EXTRA_LIBS}")
message(STATUS " CMAKE_BUILD_TYPE:                ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_LINK_EXECUTABLE        ${CMAKE_CXX_LINK_EXECUTABLE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:           ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:      ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                 ${CMAKE_CXX_FLAGS}")
endif()
