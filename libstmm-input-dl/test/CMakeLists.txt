# stmm-input-dl: test/CMakeLists.txt

# See   github.com/Barthelemy/CppProjectTemplate

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
            ${STMMI_TEST_SOURCES_DIR}/testPluginsDeviceManager.cxx
           )

    # Parameter STMMI_ADD_EVS are TRUE because the stuff is needed by
    # libstmm-input-tst88[axy] libraries
    TestFiles("${STMMI_TEST_SOURCES}" "" "stmm-input-dl;stmm-input-tst88a;stmm-input" TRUE TRUE FALSE)

    add_subdirectory(libstmm-input-tst88a)
    add_subdirectory(libstmm-input-tst88x)
    add_subdirectory(libstmm-input-tst99)

    include(CTest)
endif()
