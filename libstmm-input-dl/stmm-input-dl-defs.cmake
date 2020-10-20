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

# File:   stmm-input-dl-defs.cmake

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_DL_MAJOR_VERSION 0)
set(STMM_INPUT_DL_MINOR_VERSION 15) # !-U-!
set(STMM_INPUT_DL_VERSION "${STMM_INPUT_DL_MAJOR_VERSION}.${STMM_INPUT_DL_MINOR_VERSION}.0")

set(STMM_INPUT_DL_REQ_STMM_INPUT_BASE_VERSION "0.15") # !-U-!

include("${PROJECT_SOURCE_DIR}/../libstmm-input-base/stmm-input-base-defs.cmake")

# include dirs
list(APPEND STMMINPUTDL_EXTRA_INCLUDE_DIRS "${STMMINPUTBASE_INCLUDE_DIRS}")

set(STMMI_TEMP_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-input-dl/include")

list(APPEND STMMINPUTDL_INCLUDE_DIRS "${STMMI_TEMP_INCLUDE_DIR}")
list(APPEND STMMINPUTDL_INCLUDE_DIRS "${STMMINPUTDL_EXTRA_INCLUDE_DIRS}")

# libs
set(        STMMI_TEMP_EXTERNAL_LIBRARIES         "")
list(APPEND STMMI_TEMP_EXTERNAL_LIBRARIES         "dl")

list(APPEND STMMINPUTDL_EXTRA_LIBRARIES      "${STMMINPUTBASE_LIBRARIES}")
list(APPEND STMMINPUTDL_EXTRA_LIBRARIES      "${STMMI_TEMP_EXTERNAL_LIBRARIES}")

if (BUILD_SHARED_LIBS)
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-input-dl/build/libstmm-input-dl.so")
else()
    set(STMMI_LIB_FILE "${PROJECT_SOURCE_DIR}/../libstmm-input-dl/build/libstmm-input-dl.a")
endif()

list(APPEND STMMINPUTDL_LIBRARIES "${STMMI_LIB_FILE}")
list(APPEND STMMINPUTDL_LIBRARIES "${STMMINPUTDL_EXTRA_LIBRARIES}")

include(GNUInstallDirs)
# Usually:
#   CMAKE_INSTALL_SYSCONFDIR=etc
#   CMAKE_INSTALL_FULL_SYSCONFDIR=/etc   or  /usr/local/etc
if (NOT ("$ENV{STMM_SNAP_PACKAGING}" STREQUAL "ON"))
    set(STMMI_PLUGINS_USER_DATA_DIR ".local/${CMAKE_INSTALL_SYSCONFDIR}/stmm-input-dl")
    # if the current install path is /usr/local the following are the same
    set(STMMI_PLUGINS_DATA_DIR0     "/usr/local/${CMAKE_INSTALL_SYSCONFDIR}/stmm-input-dl")
    set(STMMI_PLUGINS_DATA_DIR1     "${CMAKE_INSTALL_FULL_SYSCONFDIR}/stmm-input-dl")
else()
    # the following will be prefixed with $SNAP_USER_DATA at runtime (ex. /snap/stmg/x4)
    set(STMMI_PLUGINS_USER_DATA_DIR "${CMAKE_INSTALL_SYSCONFDIR}/stmm-input-dl")
    # the following will be prefixed with $SNAP           at runtime (ex. /home/myuser/snap/stmg/x4)
    set(STMMI_PLUGINS_DATA_DIR0     "${CMAKE_INSTALL_FULL_SYSCONFDIR}/stmm-input-dl")
endif()

if ("${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "")
    DefineAsSecondaryTarget(stmm-input-dl  ${STMMI_LIB_FILE}  "${STMMINPUTDL_INCLUDE_DIRS}"  "stmm-input-base" "${STMMI_TEMP_EXTERNAL_LIBRARIES}")
endif()
