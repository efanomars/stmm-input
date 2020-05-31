# File: libstmm-input-base/stmm-input-base-defs.cmake

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
set(STMM_INPUT_BASE_MAJOR_VERSION 0)
set(STMM_INPUT_BASE_MINOR_VERSION 13) # !-U-!
set(STMM_INPUT_BASE_VERSION "${STMM_INPUT_BASE_MAJOR_VERSION}.${STMM_INPUT_BASE_MINOR_VERSION}.0")

set(STMM_INPUT_BASE_REQ_STMM_INPUT_VERSION "0.13") # !-U-!

include("${PROJECT_SOURCE_DIR}/../libstmm-input/stmm-input-defs.cmake")

# include dirs
list(APPEND STMMINPUTBASE_EXTRA_INCLUDE_DIRS "${STMMINPUT_INCLUDE_DIRS}")

set(STMMI_TEMP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-input-base/include")

list(APPEND STMMINPUTBASE_INCLUDE_DIRS "${STMMI_TEMP_INCLUDE_DIR}")
list(APPEND STMMINPUTBASE_INCLUDE_DIRS "${STMMINPUTBASE_EXTRA_INCLUDE_DIRS}")

# libs
set(        STMMI_TEMP_EXTERNAL_LIBRARIES      "")

set(        STMMINPUTBASE_EXTRA_LIBRARIES      "")
list(APPEND STMMINPUTBASE_EXTRA_LIBRARIES      "${STMMINPUT_LIBRARIES}")
list(APPEND STMMINPUTBASE_EXTRA_LIBRARIES      "${STMMI_TEMP_EXTERNAL_LIBRARIES}")

if (BUILD_SHARED_LIBS)
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-input-base/build/libstmm-input-base.so")
else()
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-input-base/build/libstmm-input-base.a")
endif()

list(APPEND STMMINPUTBASE_LIBRARIES "${STMMI_LIB_FILE}")
list(APPEND STMMINPUTBASE_LIBRARIES "${STMMINPUTBASE_EXTRA_LIBRARIES}")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    DefineAsSecondaryTarget(stmm-input-base  ${STMMI_LIB_FILE}  "${STMMINPUTBASE_INCLUDE_DIRS}"  "stmm-input" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
endif()

# if (("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "") AND NOT TARGET stmm-input-base)
#     if (BUILD_SHARED_LIBS)
#         add_library(stmm-input-base SHARED IMPORTED)
#     else()
#         add_library(stmm-input-base STATIC IMPORTED)
#     endif()
#     set_target_properties(stmm-input-base PROPERTIES IMPORTED_LOCATION             "${STMMI_LIB_FILE}")
#     set_target_properties(stmm-input-base PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${STMMINPUTBASE_INCLUDE_DIRS}")
#     set(STMMI_TEMP_PROPS "")
#     get_target_property(STMMI_TEMP_INPUT_PROP stmm-input INTERFACE_LINK_LIBRARIES)
#     list(APPEND STMMI_TEMP_PROPS "${STMMI_TEMP_INPUT_PROP}")
#     list(APPEND STMMI_TEMP_PROPS "stmm-input")
#     set_target_properties(stmm-input-base PROPERTIES INTERFACE_LINK_LIBRARIES      "${STMMI_TEMP_PROPS}")
# endif()
