# stmm-input-dl: test/CMakeLists.txt

#  Copyright © 2017-2018  Stefano Marsili, <stemars@gmx.ch>
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

include(CommonTesting)

option(BUILD_TESTING "Build tests" OFF)

if (NOT BUILD_SHARED_LIBS)
    set(BUILD_TESTING OFF)
endif()

if (BUILD_TESTING)
    # Test dirs
    set(STMMI_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")
    # Test sources should end with .cxx
    set(STMMI_TEST_SOURCES
            "${STMMI_TEST_SOURCES_DIR}/testPluginsDeviceManager.cxx"
           )

    # Parameter STMMI_ADD_EVS is TRUE because the stuff is needed by
    # libstmm-input-tst88[axy] libraries
    TestFiles("${STMMI_TEST_SOURCES}" "" "" "stmm-input-dl;stmm-input-tst88a" TRUE TRUE FALSE)

    add_subdirectory(libstmm-input-tst88a)
    add_subdirectory(libstmm-input-tst88x)
    add_subdirectory(libstmm-input-tst99)

    include(CTest)
endif()
