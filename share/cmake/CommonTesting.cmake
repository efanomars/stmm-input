# share/cmake/CommonTesting.cmake

# Modified from Barthélémy von Haller's  github.com/Barthelemy/CppProjectTemplate

# STMMI_TARGET_LIBS    list of (shared) libraries that have to be linked to each test
#                      ex. for "stmm-input-ev;stmm-input-gtk", omit to prepend 'lib'!
# STMMI_ADD_FAKES      bool that tells whether the (headers) in 'share/test/' should be
#                      included in the compilation of the tests
# STMMI_TEST_SOURCES   list of test source files
function(TestFiles STMMI_TARGET_LIBS  STMMI_ADD_FAKES  STMMI_TEST_SOURCES)

    set(BUILD_TESTING_GOOGLE_TEST_LIBRARY_PATH "/usr/lib"
          CACHE PATH "The path to the GoogleTest shared libraries (gtest.so and gtest_main.so)")
    mark_as_advanced(BUILD_TESTING_GOOGLE_TEST_LIBRARY_PATH)
    set(BUILD_TESTING_GOOGLE_TEST_INCLUDE_PATH "/usr/include"
          CACHE PATH "The path to the GoogleTest include dir 'gtest/'")
    mark_as_advanced(BUILD_TESTING_GOOGLE_TEST_INCLUDE_PATH)

    if (BUILD_TESTING)
        set(STMMI_DO_TESTING TRUE)

        find_library(STMMIFINDGTEST   gtest   PATHS ${BUILD_TESTING_GOOGLE_TEST_LIBRARY_PATH})
        if ("${STMMIFINDGTEST}" STREQUAL "STMMIFINDGTEST-NOTFOUND")
            set(STMMI_DO_TESTING FALSE)
            message(WARNING "Couldn't find library gtest")
        endif ("${STMMIFINDGTEST}" STREQUAL "STMMIFINDGTEST-NOTFOUND")
        mark_as_advanced(STMMIFINDGTEST)

        find_library(STMMIFINDGTESTMAIN   gtest_main   PATHS ${BUILD_TESTING_GOOGLE_TEST_LIBRARY_PATH})
        if ("${STMMIFINDGTESTMAIN}" STREQUAL "STMMIFINDGTESTMAIN-NOTFOUND")
            set(STMMI_DO_TESTING FALSE)
            message(WARNING "Couldn't find library gtest_main")
        endif ("${STMMIFINDGTESTMAIN}" STREQUAL "STMMIFINDGTESTMAIN-NOTFOUND")
        mark_as_advanced(STMMIFINDGTESTMAIN)

        find_file(STMMIFINDGTESTH  "gtest/gtest.h"  PATHS ${BUILD_TESTING_GOOGLE_TEST_INCLUDE_PATH}
                    NO_CMAKE_PATH  NO_CMAKE_ENVIRONMENT_PATH)
        if ("${STMMIFINDGTESTH}" STREQUAL "STMMIFINDGTESTH-NOTFOUND")
            set(STMMI_DO_TESTING FALSE)
            message(WARNING "Couldn't find file 'gtest/gtest.h'")
        else ("${STMMIFINDGTESTH}" STREQUAL "STMMIFINDGTESTH-NOTFOUND")
            get_filename_component(GTESTHDIR  "${STMMIFINDGTESTH}"  DIRECTORY)
            get_filename_component(GTESTINCLUDEDIR  "${GTESTHDIR}"  DIRECTORY)
        endif ("${STMMIFINDGTESTH}" STREQUAL "STMMIFINDGTESTH-NOTFOUND")
        mark_as_advanced(STMMIFINDGTESTH)
        #message(STATUS "BUILD_TESTING_GOOGLE_TEST_LIBRARY_PATH         ${BUILD_TESTING_GOOGLE_TEST_LIBRARY_PATH}")
        #message(STATUS "BUILD_TESTING_GOOGLE_TEST_INCLUDE_PATH         ${BUILD_TESTING_GOOGLE_TEST_INCLUDE_PATH}")
        #message(STATUS "GTESTHDIR         ${GTESTHDIR}")
        #message(STATUS "GTESTINCLUDEDIR   ${GTESTINCLUDEDIR}")
        #message(STATUS "STMMIFINDGTEST         ${STMMIFINDGTEST}")
        #message(STATUS "STMMIFINDGTESTMAIN     ${STMMIFINDGTESTMAIN}")
        #message(STATUS "STMMIFINDGTESTH        ${STMMIFINDGTESTH}")
        #message(STATUS "STMMI_DO_TESTING  ${STMMI_DO_TESTING}")
        #message(STATUS "CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}")
        #message(STATUS "PROJECT_SOURCE_DIR        ${PROJECT_SOURCE_DIR}")
        #message(STATUS "STMMI_TARGET_LIBS    ${STMMI_TARGET_LIBS}")
        #message(STATUS "STMMI_ADD_FAKES      ${STMMI_ADD_FAKES}")
        #message(STATUS "STMMI_TEST_SOURCES   ${STMMI_TEST_SOURCES}")
        #message(STATUS "ARGC                 ${ARGC}")
        #message(STATUS "ARGN                 ${ARGN}")
        #message(STATUS "ARGV                 ${ARGV}")
        #message(STATUS "ARGV0                ${ARGV0}")

        if (STMMI_DO_TESTING)
            # Iterate over all tests found. For each, declare an executable and add it to the tests list.
            foreach (STMMI_TEST_CUR_FILE  ${STMMI_TEST_SOURCES})

                file(RELATIVE_PATH  STMMI_TEST_CUR_REL_FILE  ${PROJECT_SOURCE_DIR}/test  ${STMMI_TEST_CUR_FILE})

                string(REGEX REPLACE "[./]" "_" STMMI_TEST_CUR_TGT ${STMMI_TEST_CUR_REL_FILE})

                add_executable(${STMMI_TEST_CUR_TGT} ${STMMI_TEST_CUR_FILE} ${STMMI_TEST_SOURCES_HELPER})

                target_include_directories(${STMMI_TEST_CUR_TGT} BEFORE PRIVATE ${PROJECT_SOURCE_DIR}/src ${GTESTINCLUDEDIR}) # add private includes
                if (STMMI_ADD_FAKES)
                    target_include_directories(${STMMI_TEST_CUR_TGT} BEFORE PRIVATE ${PROJECT_SOURCE_DIR}/../share/test)
                endif (STMMI_ADD_FAKES)

                target_link_libraries(${STMMI_TEST_CUR_TGT} ${STMMIFINDGTESTMAIN} ${STMMIFINDGTEST}) # link against gtest libs
                foreach (STMMI_TEST_CUR_LIB  ${STMMI_TARGET_LIBS})
                    target_link_libraries(${STMMI_TEST_CUR_TGT}  ${STMMI_TEST_CUR_LIB})
                endforeach (STMMI_TEST_CUR_LIB  ${STMMI_TARGET_LIBS})

                add_test(NAME ${STMMI_TEST_CUR_TGT} COMMAND ${STMMI_TEST_CUR_TGT})       # this is how to add tests to CMake

            endforeach (STMMI_TEST_CUR_FILE  ${STMMI_TEST_SOURCES})
        endif (STMMI_DO_TESTING)
    endif (BUILD_TESTING)

endfunction(TestFiles STMMI_TARGET_LIBS  STMMI_ADD_FAKES  STMMI_TEST_SOURCES)
