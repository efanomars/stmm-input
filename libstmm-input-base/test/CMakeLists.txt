# stmm-input-base: test/CMakeLists.txt

# See   github.com/Barthelemy/CppProjectTemplate

include(CommonTesting)

option(BUILD_TESTING "Build tests" OFF)

if (BUILD_TESTING)
    # Tests dir
    set(STMMI_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")
    # Test sources need to end with .cxx
    set(STMMI_TEST_SOURCES
            "${STMMI_TEST_SOURCES_DIR}/testCallIfSimplifier.cxx"
          )

    TestFiles("${STMMI_TEST_SOURCES}" "" "stmm-input-base" TRUE TRUE FALSE)

    include(CTest)
endif()
