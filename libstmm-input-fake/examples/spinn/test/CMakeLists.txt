# spinn: test/CMakeLists.txt

option(BUILD_TESTING "Build tests" OFF)

# Tests dir
set(SPINN_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")
# Test sources need to end with .cxx
set(SPINN_TEST_SOURCES
        ${SPINN_TEST_SOURCES_DIR}/testSpinn.cxx
      )
set(SPINN_TEST_WITH_SOURCES
        ${PROJECT_SOURCE_DIR}/src/spinn.h
        ${PROJECT_SOURCE_DIR}/src/spinn.cc
      )

if (BUILD_TESTING)
    #
    add_executable(spinn-test ${SPINN_TEST_SOURCES_DIR}/testSpinn.cxx ${SPINN_TEST_WITH_SOURCES})

    # Required libraries
    pkg_check_modules(STMMINPUTFAKE  REQUIRED  stmm-input-fake>=0.14) # !-U-!

    pkg_check_modules(SIGCPP  REQUIRED  sigc++-2.0>=2.4.0)

    list(APPEND SPINN_TEST_EXTRA_INCLUDE_DIRS   ${STMMINPUTFAKE_INCLUDE_DIRS} ${SIGCPP_INCLUDE_DIRS})
    list(APPEND SPINN_TEST_EXTRA_LIBS           ${STMMINPUTFAKE_LIBRARIES}    ${SIGCPP_LIBRARIES})

    if ($ENV{STMM_CMAKE_COMMENTS})
    message(STATUS "")
    message(STATUS "spinn-test was configured with the following options:")
    message(STATUS " SPINN_TEST_EXTRA_INCLUDE_DIRS:       ${SPINN_TEST_EXTRA_INCLUDE_DIRS}")
    message(STATUS " SPINN_TEST_EXTRA_LIBS:               ${SPINN_TEST_EXTRA_LIBS}")
    message(STATUS " SPINN_TEST_WITH_SOURCES:             ${SPINN_TEST_WITH_SOURCES}")
    message(STATUS " SPINN_TEST_WITH_SOURCES:             ${SPINN_TEST_WITH_SOURCES}")
    message(STATUS " BUILD_SHARED_LIBS:                   ${BUILD_SHARED_LIBS}")
    endif()

    target_include_directories(spinn-test BEFORE PRIVATE ${SPINN_TEST_EXTRA_INCLUDE_DIRS})
    target_include_directories(spinn-test BEFORE PRIVATE ${PROJECT_SOURCE_DIR}/src)

    target_link_libraries(spinn-test  ${SPINN_TEST_EXTRA_LIBS})
    target_link_libraries(spinn-test  "${SPINN_EXTRA_LIBS}")

    set_target_properties(spinn-test PROPERTIES OUTPUT_NAME testSpinn)

    add_test(NAME spinn-test COMMAND testSpinn)

endif (BUILD_TESTING)

include(CTest)
