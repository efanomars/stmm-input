# File: stmm-input-plugins/stmm-input-plugins-defs.cmake

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
set(STMM_INPUT_PLUGINS_MAJOR_VERSION 0)
set(STMM_INPUT_PLUGINS_MINOR_VERSION 13) # !-U-!
set(STMM_INPUT_PLUGINS_VERSION "${STMM_INPUT_PLUGINS_MAJOR_VERSION}.${STMM_INPUT_PLUGINS_MINOR_VERSION}.0")

# Just for the plugins directories
include("${PROJECT_SOURCE_DIR}/../libstmm-input-dl/stmm-input-dl-defs.cmake")

# include dirs
list(APPEND STMMINPUTPLUGINS_EXTRA_INCLUDE_DIRS  "")

# libs
list(APPEND STMMINPUTPLUGINS_EXTRA_LIBRARIES     "")
