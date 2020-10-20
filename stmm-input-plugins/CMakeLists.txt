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

# File:   stmm-input-plugins/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

option(STMM_INSTALL_MAN_PAGE "Install man page in debian.orig/" ON)


project(stmm-input-plugins CXX)

set(RUNTIME_OUTPUT_DIRECTORY "build")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../share/cmake")

include(CommonUtil)

CheckBinaryNotSourceTree()
CheckBuildType()
DefineCommonCompileOptions("c++14")

# Source files (and headers only used for building)
set(STMMI_PLUGINS_SOURCES
        "${PROJECT_SOURCE_DIR}/src/config.h"
        "${PROJECT_SOURCE_DIR}/src/main.cc"
        "${PROJECT_SOURCE_DIR}/src/sysdata.h"
        "${PROJECT_SOURCE_DIR}/src/sysdata.cc"
        )

add_executable(stmm-input-plugins ${STMMI_PLUGINS_SOURCES}
        "${PROJECT_BINARY_DIR}/config.cc"
        "${PROJECT_BINARY_DIR}/dlpinfos.cc"
        "${PROJECT_BINARY_DIR}/dlutil.cc"
        )

set(DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_STMMINPUTPLUGINS_CMAKE "THIS FILE WAS AUTOMATICALLY GENERATED! DO NOT MODIFY!")
set(STMMI_IMPORTED_DL_FILES_DIR "${PROJECT_SOURCE_DIR}/../libstmm-input-dl/src")
set(STMMI_IMPORTED_DL_FILES
        "${PROJECT_BINARY_DIR}/dlpinfos.h"
        "${PROJECT_BINARY_DIR}/dlpinfos.cc"
        "${PROJECT_BINARY_DIR}/dlutil.h"
        "${PROJECT_BINARY_DIR}/dlutil.cc"
     )
foreach (STMMI_CUR_IMPORTED_DL_FILE  ${STMMI_IMPORTED_DL_FILES})
    get_filename_component(STMMI_CUR_IMPORTED_DL_FILE_NAME "${STMMI_CUR_IMPORTED_DL_FILE}" NAME)
    configure_file("${STMMI_IMPORTED_DL_FILES_DIR}/${STMMI_CUR_IMPORTED_DL_FILE_NAME}"
                   "${STMMI_CUR_IMPORTED_DL_FILE}" @ONLY)
endforeach()

include("stmm-input-plugins-defs.cmake")

target_include_directories(stmm-input-plugins SYSTEM PUBLIC ${STMMINPUTPLUGINS_EXTRA_INCLUDE_DIRS})
# This allows config.cc to find the config.h include
target_include_directories(stmm-input-plugins        PUBLIC "${PROJECT_SOURCE_DIR}/src")

target_include_directories(stmm-input-plugins BEFORE PRIVATE ${PROJECT_BINARY_DIR})

target_link_libraries(stmm-input-plugins ${STMMINPUTPLUGINS_EXTRA_LIBRARIES})
target_link_libraries(stmm-input-plugins -lpthread)

DefineTargetPublicCompileOptions(stmm-input-plugins)

include(GNUInstallDirs)

# Set version for stmm-input-dl-config.cc.in
set(STMMI_PKG_VERSION "${STMM_INPUT_PLUGINS_VERSION}")
# Set plugin directories for stmm-input-dl-config.cc.in
set(STMMI_PKG_DATA_DIR0 "${STMMI_PLUGINS_DATA_DIR0}")
set(STMMI_PKG_DATA_DIR1 "${STMMI_PLUGINS_DATA_DIR1}")
set(STMMI_PKG_USER_DATA_DIR "${STMMI_PLUGINS_USER_DATA_DIR}")

# Create config file for executable
configure_file("${PROJECT_SOURCE_DIR}/src/config.cc.in"
               "${PROJECT_BINARY_DIR}/config.cc" @ONLY)
if (STMM_INSTALL_MAN_PAGE)
    configure_file("${PROJECT_SOURCE_DIR}/../scripts/priv/debian.orig/stmm-input-plugins.1.in"
                   "${PROJECT_BINARY_DIR}/stmm-input-plugins.1" @ONLY)
    CreateManGz(stmm-input-plugins "stmm-input-plugins.1")
endif()

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input-plugins was configured with the following options:")
message(STATUS " STMMI_PLUGINS_SOURCES:               ${STMMI_PLUGINS_SOURCES}")
message(STATUS " STMMINPUTPLUGINS_EXTRA_INCLUDE_DIRS: ${STMMINPUTPLUGINS_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMINPUTPLUGINS_EXTRA_LIBRARIES:    ${STMMINPUTPLUGINS_EXTRA_LIBRARIES}")
message(STATUS " STMMI_PLUGINS_DATA_FILES             ${STMMI_PLUGINS_DATA_FILES}")
message(STATUS " CMAKE_BUILD_TYPE:                    ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:               ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:          ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " install prefix:                      ${CMAKE_INSTALL_PREFIX}")
endif()

install(TARGETS stmm-input-plugins RUNTIME DESTINATION "bin")

if (STMM_INSTALL_MAN_PAGE)
    install(FILES                   "${PROJECT_BINARY_DIR}/stmm-input-plugins.1.gz"
            DESTINATION "${CMAKE_INSTALL_DATADIR}/man/man1/")
endif()
