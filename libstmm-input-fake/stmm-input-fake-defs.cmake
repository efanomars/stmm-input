# File: libstmm-input-fake/stmm-input-fake-defs.cmake

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
set(STMM_INPUT_FAKE_MAJOR_VERSION 0)
set(STMM_INPUT_FAKE_MINOR_VERSION 14) # !-U-!
set(STMM_INPUT_FAKE_VERSION "${STMM_INPUT_FAKE_MAJOR_VERSION}.${STMM_INPUT_FAKE_MINOR_VERSION}.0")

set(STMM_INPUT_FAKE_REQ_STMM_INPUT_EV_VERSION "0.14") # !-U-!

include("${PROJECT_SOURCE_DIR}/../libstmm-input-ev/stmm-input-ev-defs.cmake")

# include dirs
list(APPEND STMMINPUTFAKE_EXTRA_INCLUDE_DIRS  "${STMMINPUTEV_INCLUDE_DIRS}")

set(STMMI_TEMP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-input-fake/include")

list(APPEND STMMINPUTFAKE_INCLUDE_DIRS  "${STMMI_TEMP_INCLUDE_DIR}")
list(APPEND STMMINPUTFAKE_INCLUDE_DIRS  "${STMMINPUTFAKE_EXTRA_INCLUDE_DIRS}")

# libs
set(        STMMI_TEMP_EXTERNAL_LIBRARIES      "")

set(        STMMINPUTFAKE_EXTRA_LIBRARIES      "")
list(APPEND STMMINPUTFAKE_EXTRA_LIBRARIES      "${STMMINPUTEV_LIBRARIES}")
list(APPEND STMMINPUTFAKE_EXTRA_LIBRARIES      "${STMMI_TEMP_EXTERNAL_LIBRARIES}")

if (BUILD_SHARED_LIBS)
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-input-fake/build/libstmm-input-fake.so")
else()
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-input-fake/build/libstmm-input-fake.a")
endif()

list(APPEND STMMINPUTFAKE_LIBRARIES "${STMMI_LIB_FILE}")
list(APPEND STMMINPUTFAKE_LIBRARIES "${STMMINPUTFAKE_EXTRA_LIBRARIES}")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    DefineAsSecondaryTarget(stmm-input-fake  ${STMMI_LIB_FILE}  "${STMMINPUTFAKE_INCLUDE_DIRS}"  "stmm-input-ev" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
endif()

# if (("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "") AND NOT TARGET stmm-input-fake)
#     if (BUILD_SHARED_LIBS)
#         add_library(stmm-input-fake SHARED IMPORTED)
#     else()
#         add_library(stmm-input-fake STATIC IMPORTED)
#     endif()
#     set_target_properties(stmm-input-fake PROPERTIES IMPORTED_LOCATION             "${STMMI_LIB_FILE}")
#     set_target_properties(stmm-input-fake PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${STMMINPUTFAKE_INCLUDE_DIRS}")
#     set(STMMI_TEMP_PROPS "")
#     get_target_property(STMMI_TEMP_INPUTEV_PROP stmm-input-ev INTERFACE_LINK_LIBRARIES)
#     list(APPEND STMMI_TEMP_PROPS "${STMMI_TEMP_INPUTEV_PROP}")
#     list(APPEND STMMI_TEMP_PROPS "stmm-input-ev")
#     set_target_properties(stmm-input-fake PROPERTIES INTERFACE_LINK_LIBRARIES      "${STMMI_TEMP_PROPS}")
# endif()
