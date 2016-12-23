# stmm-input-gtk: test/CMakeLists.txt

include(CommonTesting)

option(BUILD_TESTING "Build tests" OFF)

# Tests dir
set(STMMI_GTK_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")
# Test sources need to end with .cxx, helper sources with .h .cc
set(STMMI_GTK_TEST_SOURCES_DATA
        ${STMMI_GTK_TEST_SOURCES_DIR}/testGdkKeyConverterEvdev.cxx
        )
set(STMMI_GTK_TEST_SOURCES_ALL
        )
# Concatenate source file lists
list(APPEND  STMMI_GTK_TEST_SOURCES
        ${STMMI_GTK_TEST_SOURCES_DATA}
        ${STMMI_GTK_TEST_SOURCES_ALL}
        )

TestFiles("${STMMI_GTK_TEST_SOURCES}" ""
        "stmm-input-gtk"
        FALSE FALSE FALSE)

set(STMMI_GTK_TEST_SOURCES_MAS
        ${STMMI_GTK_TEST_SOURCES_DIR}/testMasGtkDeviceManager.cxx
        ${STMMI_GTK_TEST_SOURCES_DIR}/testMasGtkKeyboardDevice.cxx
        ${STMMI_GTK_TEST_SOURCES_DIR}/testMasGtkPointerDevice.cxx
        )

set(STMMI_GTK_TEST_WITH_SOURCES_MAS
        ${STMMI_GTK_TEST_SOURCES_DIR}/fakemasgtkbackend.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fakemasgtkwindowdata.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fakemasgtkdevicemanager.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fixtureGlibApp.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fixtureMasDM.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fixturevariantKeyRepeatMode.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fixturevariantEventClasses.h
        )

TestFiles("${STMMI_GTK_TEST_SOURCES_MAS}"
          "${STMMI_SOURCES_DATA};${STMMI_SOURCES_MAS};${STMMI_GTK_TEST_WITH_SOURCES_MAS}"
          "${STMMI_GTK_EXTRA_LIBS}" FALSE FALSE TRUE)

set(STMMI_GTK_TEST_SOURCES_FLO
        ${STMMI_GTK_TEST_SOURCES_DIR}/testFloGtkDeviceManager.cxx
        )

set(STMMI_GTK_TEST_WITH_SOURCES_FLO
        ${STMMI_GTK_TEST_SOURCES_DIR}/fakeflogtkbackend.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fakeflogtkbackend.cc
        ${STMMI_GTK_TEST_SOURCES_DIR}/fakeflogtkwindowdata.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fakeflogtkwindowdata.cc
        ${STMMI_GTK_TEST_SOURCES_DIR}/fakeflogtkdevicemanager.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fixtureFloDM.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fixtureGlibApp.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fixturevariantKeyRepeatMode.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fixturevariantEventClasses.h
        )

TestFiles("${STMMI_GTK_TEST_SOURCES_FLO}"
          "${STMMI_SOURCES_DATA};${STMMI_SOURCES_FLO};${STMMI_GTK_TEST_WITH_SOURCES_FLO}"
          "${STMMI_GTK_EXTRA_LIBS}" FALSE FALSE TRUE)

set(STMMI_GTK_TEST_SOURCES_JS
        ${STMMI_GTK_TEST_SOURCES_DIR}/testJsGtkDeviceManager.cxx
        )

set(STMMI_GTK_TEST_WITH_SOURCES_JS
        ${STMMI_GTK_TEST_SOURCES_DIR}/fakejsgtkbackend.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fakejsgtkwindowdata.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fakejsgtkdevicemanager.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fixtureGlibApp.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fixtureJsDM.h
        ${STMMI_GTK_TEST_SOURCES_DIR}/fixturevariantEventClasses.h
        )

TestFiles("${STMMI_GTK_TEST_SOURCES_JS}"
          "${STMMI_SOURCES_DATA};${STMMI_SOURCES_JS};${STMMI_GTK_TEST_WITH_SOURCES_JS}"
          "${STMMI_GTK_EXTRA_LIBS}" FALSE FALSE TRUE)

set(STMMI_GTK_TEST_SOURCES_ALL
        ${STMMI_GTK_TEST_SOURCES_DIR}/testAllGtkDeviceManager.cxx
        )

set(STMMI_GTK_TEST_WITH_SOURCES_ALL
        ${STMMI_GTK_TEST_WITH_SOURCES_MAS}
        ${STMMI_GTK_TEST_WITH_SOURCES_FLO}
        ${STMMI_GTK_TEST_WITH_SOURCES_JS}
        ${STMMI_GTK_TEST_SOURCES_DIR}/fixtureAllDM.h
        )

TestFiles("${STMMI_GTK_TEST_SOURCES_ALL}"
          "${STMMI_SOURCES_DATA};${STMMI_SOURCES_MAS};${STMMI_SOURCES_FLO};${STMMI_SOURCES_JS};${STMMI_GTK_TEST_WITH_SOURCES_ALL}"
          "${STMMI_GTK_EXTRA_LIBS}" FALSE FALSE TRUE)

include(CTest)
