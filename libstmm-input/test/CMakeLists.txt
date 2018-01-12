# stmm-input: test/CMakeLists.txt

# See   github.com/Barthelemy/CppProjectTemplate

include(CommonTesting)

option(BUILD_TESTING "Build tests" OFF)

if (BUILD_TESTING)
    # Tests dir
    set(STMMI_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")
    # Test sources end with .cxx

    # Test sources that do not need events
    set(STMMI_TEST_SOURCES_NO_EVS
            ${STMMI_TEST_SOURCES_DIR}/testHardwareKeys.cxx
          )

    TestFiles("${STMMI_TEST_SOURCES_NO_EVS}" "" "stmm-input" FALSE FALSE FALSE)

    include(CTest)
endif()
