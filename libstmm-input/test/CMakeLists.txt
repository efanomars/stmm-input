# stmm-input: test/CMakeLists.txt

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

if (BUILD_TESTING)
    # Tests dir
    set(STMMI_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")
    # Test sources end with .cxx

    # Test sources that do not need events
    set(STMMI_TEST_SOURCES_NO_EVS
            "${STMMI_TEST_SOURCES_DIR}/testHardwareKeys.cxx"
          )

    TestFiles("${STMMI_TEST_SOURCES_NO_EVS}" "" "" "stmm-input" FALSE FALSE FALSE)

    include(CTest)
endif()
