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

# File:   stmm-input-base-defs.cmake

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_BASE_MAJOR_VERSION 0)
set(STMM_INPUT_BASE_MINOR_VERSION 16) # !-U-!
set(STMM_INPUT_BASE_VERSION "${STMM_INPUT_BASE_MAJOR_VERSION}.${STMM_INPUT_BASE_MINOR_VERSION}.0")

set(STMM_INPUT_BASE_REQ_STMM_INPUT_VERSION "0.16") # !-U-!

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
