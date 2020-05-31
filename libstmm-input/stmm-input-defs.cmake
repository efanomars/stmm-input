# File: libstmm-input/stmm-input-defs.cmake

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

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_MAJOR_VERSION 0)
set(STMM_INPUT_MINOR_VERSION 13) # !-U-!
set(STMM_INPUT_VERSION "${STMM_INPUT_MAJOR_VERSION}.${STMM_INPUT_MINOR_VERSION}.0")

# include dirs
list(APPEND STMMINPUT_EXTRA_INCLUDE_DIRS  "")

set(STMMI_TEMP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-input/include")

list(APPEND STMMINPUT_INCLUDE_DIRS "${STMMI_TEMP_INCLUDE_DIR}")
list(APPEND STMMINPUT_INCLUDE_DIRS "${STMMINPUT_EXTRA_INCLUDE_DIRS}")

# libs
set(        STMMI_TEMP_EXTERNAL_LIBRARIES     "")

set(        STMMINPUT_EXTRA_LIBRARIES         "")
list(APPEND STMMINPUT_EXTRA_LIBRARIES         "${STMMI_TEMP_EXTERNAL_LIBRARIES}")

if (BUILD_SHARED_LIBS)
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-input/build/libstmm-input.so")
else()
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-input/build/libstmm-input.a")
endif()

list(APPEND STMMINPUT_LIBRARIES "${STMMI_LIB_FILE}")
list(APPEND STMMINPUT_LIBRARIES "${STMMINPUT_EXTRA_LIBRARIES}")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    DefineAsSecondaryTarget(stmm-input  ${STMMI_LIB_FILE}  "${STMMINPUT_INCLUDE_DIRS}"  "" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
endif()

# if (("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "") AND NOT TARGET stmm-input)
#     if (BUILD_SHARED_LIBS)
#         add_library(stmm-input SHARED IMPORTED)
#     else()
#         add_library(stmm-input STATIC IMPORTED)
#     endif()
#     set_target_properties(stmm-input PROPERTIES IMPORTED_LOCATION             "${STMMI_LIB_FILE}")
#     set_target_properties(stmm-input PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${STMMINPUT_INCLUDE_DIRS}")
#     set(STMMI_TEMP_PROPS "")
#     set_target_properties(stmm-input PROPERTIES INTERFACE_LINK_LIBRARIES      "${STMMI_TEMP_PROPS}")
# endif()
