# File: libstmm-input-ev/stmm-input-ev-defs.cmake

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
set(STMM_INPUT_EV_MAJOR_VERSION 0)
set(STMM_INPUT_EV_MINOR_VERSION 13) # !-U-!
set(STMM_INPUT_EV_VERSION "${STMM_INPUT_EV_MAJOR_VERSION}.${STMM_INPUT_EV_MINOR_VERSION}.0")

set(STMM_INPUT_EV_REQ_STMM_INPUT_BASE_VERSION "0.13") # !-U-!

include("${PROJECT_SOURCE_DIR}/../libstmm-input-base/stmm-input-base-defs.cmake")

# include dirs
list(APPEND STMMINPUTEV_EXTRA_INCLUDE_DIRS "${STMMINPUTBASE_INCLUDE_DIRS}")

set(STMMI_TEMP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-input-ev/include")

list(APPEND STMMINPUTEV_INCLUDE_DIRS "${STMMI_TEMP_INCLUDE_DIR}")
list(APPEND STMMINPUTEV_INCLUDE_DIRS "${STMMINPUTEV_EXTRA_INCLUDE_DIRS}")

# libs
set(        STMMI_TEMP_EXTERNAL_LIBRARIES    "")

set(        STMMINPUTEV_EXTRA_LIBRARIES      "")
list(APPEND STMMINPUTEV_EXTRA_LIBRARIES      "${STMMINPUTBASE_LIBRARIES}")
list(APPEND STMMINPUTEV_EXTRA_LIBRARIES      "${STMMI_TEMP_EXTERNAL_LIBRARIES}")

if (BUILD_SHARED_LIBS)
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-input-ev/build/libstmm-input-ev.so")
else()
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-input-ev/build/libstmm-input-ev.a")
endif()

list(APPEND STMMINPUTEV_LIBRARIES "${STMMI_LIB_FILE}")
list(APPEND STMMINPUTEV_LIBRARIES "${STMMINPUTEV_EXTRA_LIBRARIES}")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    DefineAsSecondaryTarget(stmm-input-ev  ${STMMI_LIB_FILE}  "${STMMINPUTEV_INCLUDE_DIRS}"  "stmm-input-base" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
endif()

# if (("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "") AND NOT TARGET stmm-input-ev)
#     if (BUILD_SHARED_LIBS)
#         add_library(stmm-input-ev SHARED IMPORTED)
#     else()
#         add_library(stmm-input-ev STATIC IMPORTED)
#     endif()
#     set_target_properties(stmm-input-ev PROPERTIES IMPORTED_LOCATION             "${STMMI_LIB_FILE}")
#     set_target_properties(stmm-input-ev PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${STMMINPUTEV_INCLUDE_DIRS}")
#     set(STMMI_TEMP_PROPS "")
#     get_target_property(STMMI_TEMP_INPUTBASE_PROP stmm-input-base INTERFACE_LINK_LIBRARIES)
#     list(APPEND STMMI_TEMP_PROPS "${STMMI_TEMP_INPUTBASE_PROP}")
#     list(APPEND STMMI_TEMP_PROPS "stmm-input-base")
#     set_target_properties(stmm-input-ev PROPERTIES INTERFACE_LINK_LIBRARIES      "${STMMI_TEMP_PROPS}")
# endif()
