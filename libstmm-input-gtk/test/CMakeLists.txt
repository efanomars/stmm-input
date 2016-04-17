# stmm-input-gtk: test/CMakeLists.txt

# See   github.com/Barthelemy/CppProjectTemplate

include(CommonTesting)

option(BUILD_TESTING "Build tests" OFF)

# Tests dir
set(STMMI_GTK_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")
# Test sources need to end with .cxx, helper sources with .h .cc
set(STMMI_GTK_TEST_SOURCES_DATA
        ${STMMI_GTK_TEST_SOURCES_DIR}/testHardwareKeyConverter.cxx
        )
set(STMMI_GTK_TEST_SOURCES_MAS
        )
set(STMMI_GTK_TEST_SOURCES_FLO
        )
set(STMMI_GTK_TEST_SOURCES_JS
        )
set(STMMI_GTK_TEST_SOURCES_ALL
        )
# Concatenate source file lists
list(APPEND  STMMI_GTK_TEST_SOURCES
        ${STMMI_GTK_TEST_SOURCES_DATA}
        ${STMMI_GTK_TEST_SOURCES_MAS}
        ${STMMI_GTK_TEST_SOURCES_FLO}
        ${STMMI_GTK_TEST_SOURCES_JS}
        ${STMMI_GTK_TEST_SOURCES_ALL}
        )

TestFiles(stmm-input-gtk FALSE "${STMMI_GTK_TEST_SOURCES}")

include(CTest)
