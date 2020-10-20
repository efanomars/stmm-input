# Copyright © 2018-2020  Stefano Marsili, <stemars@gmx.ch>
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

# File:   libstmm-input-gtk-dm/test/CMakeLists.txt

include(CommonTesting)

option(BUILD_TESTING "Build tests" OFF)

if (BUILD_TESTING)

    # Tests dir
    set(STMMI_GTK_DM_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")

    set(STMMI_TST_GTK_DM_TARGET_LIST "stmm-input-gtk;stmm-input-ev")
    if (NOT OMIT_PLUGINS)
        list(APPEND STMMI_TST_GTK_DM_TARGET_LIST "stmm-input-dl")
    endif()

    # Test sources need to end with .cxx, helper sources with .h .cc
    set(STMMI_GTK_DM_TEST_SOURCES_MAS
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/testMasGtkDeviceManager.cxx
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/testMasGtkKeyboardDevice.cxx
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/testMasGtkPointerDevice.cxx
            )

    set(STMMI_GTK_DM_TEST_WITH_SOURCES_MAS
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fakemasgtkbackend.h
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fakemasgtkwindowdata.h
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fakemasgtkdevicemanager.h
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fixtureGlibApp.h
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fixtureMasDM.h
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fixtureTestBase.h
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fixturevariantKeyRepeatMode.h
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fixturevariantEventClasses.h
            )

    TestFiles("${STMMI_GTK_DM_TEST_SOURCES_MAS}"
            "${STMMI_SOURCES_DATA};${STMMI_SOURCES_MAS};${STMMI_GTK_DM_TEST_WITH_SOURCES_MAS}"
            "" "${STMMI_TST_GTK_DM_TARGET_LIST}" FALSE FALSE TRUE)

    set(STMMI_GTK_DM_TEST_SOURCES_JS
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/testJsGtkDeviceManager.cxx
            )

    set(STMMI_GTK_DM_TEST_WITH_SOURCES_JS
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fakejsgtkbackend.h
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fakejsgtkwindowdata.h
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fakejsgtkdevicemanager.h
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fixtureGlibApp.h
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fixtureJsDM.h
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fixtureTestBase.h
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fixturevariantEventClasses.h
            )

    TestFiles("${STMMI_GTK_DM_TEST_SOURCES_JS}"
            "${STMMI_SOURCES_DATA};${STMMI_SOURCES_JS};${STMMI_GTK_DM_TEST_WITH_SOURCES_JS}"
            "" "${STMMI_TST_GTK_DM_TARGET_LIST}" FALSE FALSE TRUE)

    set(STMMI_GTK_DM_TEST_SOURCES_ALL
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/testAllGtkDeviceManager.cxx
            )

    set(STMMI_GTK_DM_TEST_WITH_SOURCES_ALL
            ${STMMI_GTK_DM_TEST_WITH_SOURCES_MAS}
            ${STMMI_GTK_DM_TEST_WITH_SOURCES_FLO}
            ${STMMI_GTK_DM_TEST_WITH_SOURCES_JS}
            ${STMMI_GTK_DM_TEST_SOURCES_DIR}/fixtureAllDM.h
            )

    TestFiles("${STMMI_GTK_DM_TEST_SOURCES_ALL}"
            "${STMMI_SOURCES_DATA};${STMMI_SOURCES_MAS};${STMMI_SOURCES_FLO};${STMMI_SOURCES_JS};${STMMI_GTK_DM_TEST_WITH_SOURCES_ALL}"
            "" "${STMMI_TST_GTK_DM_TARGET_LIST}" FALSE FALSE TRUE)

    include(CTest)

endif()
