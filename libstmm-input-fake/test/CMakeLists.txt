# stmm-input-fake: test/CMakeLists.txt

include(CommonTesting)

option(BUILD_TESTING "Build tests" OFF)

if (BUILD_TESTING)

    set(TESTFAKELIBS "${STMMINPUTFAKE_EXTRA_LIBRARIES}")
    list(APPEND TESTFAKELIBS "stmm-input-fake")

    # Tests dir
    set(STMMI_FAKE_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")
    # Test sources need to end with .cxx, helper sources with .h .cc
    set(STMMI_FAKE_TEST_SOURCES
            "${STMMI_FAKE_TEST_SOURCES_DIR}/testFakeDeviceManager.cxx"
            )

    TestFiles("${STMMI_FAKE_TEST_SOURCES}" ""
              "" "${TESTFAKELIBS}"
              FALSE FALSE FALSE)

    include(CTest)
endif()
