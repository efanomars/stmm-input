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

# File:   stmm-input-gtk-dm-defs.cmake

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_GTK_DM_MAJOR_VERSION 0)
set(STMM_INPUT_GTK_DM_MINOR_VERSION 16) # !-U-!
set(STMM_INPUT_GTK_DM_VERSION "${STMM_INPUT_GTK_DM_MAJOR_VERSION}.${STMM_INPUT_GTK_DM_MINOR_VERSION}.0")

# required stmm-input-gtk version
set(STMM_INPUT_GTK_DM_REQ_STMM_INPUT_GTK_VERSION "0.16") # !-U-!

# required stmm-input-ev version
set(STMM_INPUT_GTK_DM_REQ_STMM_INPUT_EV_VERSION "0.16") # !-U-!

if (NOT OMIT_PLUGINS)
    # required stmm-input-dl version
    set(STMM_INPUT_GTK_DM_REQ_STMM_INPUT_DL_VERSION "0.16") # !-U-!
endif()

include("${PROJECT_SOURCE_DIR}/../libstmm-input-gtk/stmm-input-gtk-defs.cmake")
include("${PROJECT_SOURCE_DIR}/../libstmm-input-ev/stmm-input-ev-defs.cmake")
if (NOT OMIT_PLUGINS)
    include("${PROJECT_SOURCE_DIR}/../libstmm-input-dl/stmm-input-dl-defs.cmake")
endif()

# include dirs
list(APPEND STMMINPUTGTKDM_EXTRA_INCLUDE_DIRS  "${STMMINPUTGTK_INCLUDE_DIRS}")
list(APPEND STMMINPUTGTKDM_EXTRA_INCLUDE_DIRS  "${STMMINPUTEV_INCLUDE_DIRS}")
if (NOT OMIT_PLUGINS)
    list(APPEND STMMINPUTGTKDM_EXTRA_INCLUDE_DIRS  "${STMMINPUTDL_INCLUDE_DIRS}")
endif()

set(STMMI_TEMP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-input-gtk-dm/include")

list(APPEND STMMINPUTGTKDM_INCLUDE_DIRS  "${STMMI_TEMP_INCLUDE_DIR}")
list(APPEND STMMINPUTGTKDM_INCLUDE_DIRS  "${STMMINPUTGTKDM_EXTRA_INCLUDE_DIRS}")

# libs
set(        STMMI_TEMP_EXTERNAL_LIBRARIES       "")

set(        STMMINPUTGTKDM_EXTRA_LIBRARIES      "")
list(APPEND STMMINPUTGTKDM_EXTRA_LIBRARIES      "${STMMINPUTGTK_LIBRARIES}")
list(APPEND STMMINPUTGTKDM_EXTRA_LIBRARIES      "${STMMINPUTEV_LIBRARIES}")
if (NOT OMIT_PLUGINS)
    list(APPEND STMMINPUTGTKDM_EXTRA_LIBRARIES      "${STMMINPUTDL_LIBRARIES}")
endif()
list(APPEND STMMINPUTGTKDM_EXTRA_LIBRARIES       "${STMMI_TEMP_EXTERNAL_LIBRARIES}")

if (BUILD_SHARED_LIBS)
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-input-gtk-dm/build/libstmm-input-gtk-dm.so")
else()
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-input-gtk-dm/build/libstmm-input-gtk-dm.a")
endif()

list(APPEND STMMINPUTGTKDM_LIBRARIES "${STMMI_LIB_FILE}")
list(APPEND STMMINPUTGTKDM_LIBRARIES "${STMMINPUTGTKDM_EXTRA_LIBRARIES}")

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    if (NOT OMIT_PLUGINS)
        DefineAsSecondaryTarget(stmm-input-gtk-dm  ${STMMI_LIB_FILE}  "${STMMINPUTGTKDM_INCLUDE_DIRS}"  "stmm-input-gtk;stmm-input-ev;stmm-input-dl" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
    else()
        DefineAsSecondaryTarget(stmm-input-gtk-dm  ${STMMI_LIB_FILE}  "${STMMINPUTGTKDM_INCLUDE_DIRS}"  "stmm-input-gtk;stmm-input-ev" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
    endif()
endif()
