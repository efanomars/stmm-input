# stmm-input-gtk: test/CMakeLists.txt

include(CommonTesting)

option(BUILD_TESTING "Build tests" OFF)

if (BUILD_TESTING)

    # Tests dir
    set(STMMI_GTK_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")
    # Test sources need to end with .cxx, helper sources with .h .cc
    list(APPEND  STMMI_GTK_TEST_SOURCES
            "${STMMI_GTK_TEST_SOURCES_DIR}/testGtkAccessor.cxx"
            "${STMMI_GTK_TEST_SOURCES_DIR}/testGdkKeyConverterEvdev.cxx"
            )

    TestFiles("${STMMI_GTK_TEST_SOURCES}" ""
            "" "stmm-input-gtk"
            FALSE FALSE FALSE)

    include(CTest)

endif()
