# share/cmake/CommonTesting.cmake

# Modified from Barthélémy von Haller's  github.com/Barthelemy/CppProjectTemplate

# Parameters:
# STMMI_TEST_SOURCES   list of test source files for each of which a test executable
#                      is created.
# STMMI_WITH_SOURCES   list of sources that are compiled with each of the tests in 
#                      STMMI_TEST_SOURCES
# STMMI_TARGET_LIB     the target library or empty string if not needed
# STMMI_LINKED_LIBS    list of (shared) libraries that have to be linked to each test.
#                      One of them might be the to be tested library target name
#                      (cmake will recognize and link the freshly created one instead of 
#                      the installed one).
#                      ex. "stmm-input-ev;stmm-input-gtk". Note: don't prepend 'lib'!
# STMMI_ADD_FAKES      bool that tells whether the fake devices defined in 'libstmm-input-fake'
#                      should be compiled with the tests.
# STMMI_ADD_EVS        bool that tells whether the device capabilities and events defined
#                      in 'libstmm-input-ev' should be compiled with the tests.
# STMMI_FAKE_IFACE     bool that tells whether the compiler definition STMI_TESTING_IFACE
#                      is defined for each test. This can be used to conditionally declare 
#                      a class method virtual to create mock or fake subclasses.
#                      
#
# Implicit paramters (all the project's libraries have to define them):
# STMMI_HEADERS_DIR    The directory of public headers of the to be tested library
# STMMI_INCLUDE_DIR    The directory containing STMMI_HEADERS_DIR
# STMMI_SOURCES_DIR    The directory of private headers of the to be tested library
# 
function(TestFiles STMMI_TEST_SOURCES  STMMI_WITH_SOURCES  STMMI_TARGET_LIB  STMMI_LINKED_LIBS  STMMI_ADD_FAKES  STMMI_ADD_EVS  STMMI_FAKE_IFACE)

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
        #message(STATUS "STMMI_DO_TESTING       ${STMMI_DO_TESTING}")
        #message(STATUS "CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}")
        #message(STATUS "PROJECT_SOURCE_DIR       ${PROJECT_SOURCE_DIR}")
        message(STATUS "STMMI_LINKED_LIBS    ${STMMI_LINKED_LIBS}")
        message(STATUS "STMMI_ADD_FAKES      ${STMMI_ADD_FAKES}")
        message(STATUS "STMMI_TEST_SOURCES   ${STMMI_TEST_SOURCES}")
        message(STATUS "STMMI_WITH_SOURCES   ${STMMI_WITH_SOURCES}")
        message(STATUS "STMMI_FAKE_IFACE     ${STMMI_FAKE_IFACE}")
        message(STATUS "STMMI_HEADERS_DIR    ${STMMI_HEADERS_DIR}")
        message(STATUS "STMMI_SOURCES_DIR    ${STMMI_SOURCES_DIR}")
        #message(STATUS "ARGC                 ${ARGC}")
        #message(STATUS "ARGN                 ${ARGN}")
        #message(STATUS "ARGV                 ${ARGV}")
        #message(STATUS "ARGV0                ${ARGV0}")

        if (STMMI_DO_TESTING)
            if (STMMI_ADD_FAKES)
                set(DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE "THIS FILE WAS AUTOMATICALLY GENERATED! DO NOT MODIFY!")
                set(STMMI_FAKES_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/../libstmm-input-fake/include/stmm-input-fake")
                set(STMMI_FAKES_SOURCES
                        fakedevice.h
                        fakejoystickdevice.h
                        fakekeydevice.h
                        fakepointerdevice.h
                        faketouchdevice.h
                     )
                foreach (STMMI_TEST_CUR_FAKE_SOURCE  ${STMMI_FAKES_SOURCES})
#message(STATUS "STMMI_TEST_CUR_FAKE_SOURCE    ${STMMI_TEST_CUR_FAKE_SOURCE}")
                    configure_file("${STMMI_FAKES_INCLUDE_DIR}/${STMMI_TEST_CUR_FAKE_SOURCE}"
                                   "${PROJECT_BINARY_DIR}/${STMMI_TEST_CUR_FAKE_SOURCE}" @ONLY)
                endforeach (STMMI_TEST_CUR_FAKE_SOURCE  ${STMMI_FAKES_SOURCES})
            endif (STMMI_ADD_FAKES)
            #
            set(STMMI_EVS_SOURCES)
            if (STMMI_ADD_EVS)
              set(DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE "THIS FILE WAS AUTOMATICALLY GENERATED! DO NOT MODIFY!")
                set(STMMI_EVS_HEADERS_DIR "${PROJECT_SOURCE_DIR}/../libstmm-input-ev/include/stmm-input-ev")
                set(STMMI_EVS_HEADERS
                        "${PROJECT_BINARY_DIR}/joystickcapability.h"
                        "${PROJECT_BINARY_DIR}/joystickevent.h"
                        "${PROJECT_BINARY_DIR}/keycapability.h"
                        "${PROJECT_BINARY_DIR}/keyevent.h"
                        "${PROJECT_BINARY_DIR}/pointercapability.h"
                        "${PROJECT_BINARY_DIR}/pointerevent.h"
                        "${PROJECT_BINARY_DIR}/touchcapability.h"
                        "${PROJECT_BINARY_DIR}/touchevent.h"
                     )
                foreach (STMMI_TEST_CUR_EVS_HEADER  ${STMMI_EVS_HEADERS})
#message(STATUS "STMMI_TEST_CUR_EVS_HEADER    ${STMMI_TEST_CUR_EVS_HEADER}")
                    get_filename_component(STMMI_TEST_CUR_EVS_HEADER_NAME "${STMMI_TEST_CUR_EVS_HEADER}" NAME)
                    configure_file("${STMMI_EVS_HEADERS_DIR}/${STMMI_TEST_CUR_EVS_HEADER_NAME}"
                                   "${STMMI_TEST_CUR_EVS_HEADER}" @ONLY)
                endforeach (STMMI_TEST_CUR_EVS_HEADER  ${STMMI_EVS_HEADERS})
                set(STMMI_EVS_SOURCES_DIR "${PROJECT_SOURCE_DIR}/../libstmm-input-ev/src")
                set(STMMI_EVS_SOURCES
                        "${PROJECT_BINARY_DIR}/joystickcapability.cc"
                        "${PROJECT_BINARY_DIR}/joystickevent.cc"
                        "${PROJECT_BINARY_DIR}/keycapability.cc"
                        "${PROJECT_BINARY_DIR}/keyevent.cc"
                        "${PROJECT_BINARY_DIR}/pointercapability.cc"
                        "${PROJECT_BINARY_DIR}/pointerevent.cc"
                        "${PROJECT_BINARY_DIR}/touchcapability.cc"
                        "${PROJECT_BINARY_DIR}/touchevent.cc"
                     )
                foreach (STMMI_TEST_CUR_EVS_SOURCE  ${STMMI_EVS_SOURCES})
#message(STATUS "STMMI_TEST_CUR_EVS_SOURCE    ${STMMI_TEST_CUR_EVS_SOURCE}")
                    get_filename_component(STMMI_TEST_CUR_EVS_SOURCE_NAME "${STMMI_TEST_CUR_EVS_SOURCE}" NAME)
                    configure_file("${STMMI_EVS_SOURCES_DIR}/${STMMI_TEST_CUR_EVS_SOURCE_NAME}"
                                   "${STMMI_TEST_CUR_EVS_SOURCE}" @ONLY)
                endforeach (STMMI_TEST_CUR_EVS_SOURCE  ${STMMI_EVS_SOURCES})
            endif (STMMI_ADD_EVS)
            #  TODO
            #  #precompile a static lib with STMMI_WITH_SOURCES
            #  list(LENGTH STMMI_WITH_SOURCES STMMI_WITH_SOURCES_LEN)
            #  if (STMMI_WITH_SOURCES_LEN GREATER 0)
            #      add_library(stmmiwithsourcesobjlib OBJECT ${STMMI_WITH_SOURCES})
            #  endif (STMMI_WITH_SOURCES_LEN GREATER 0)
            #   ^ need to target_compile_definitions target_include_directories !!!
            #
            # Iterate over all tests found. For each, declare an executable and add it to the tests list.
            foreach (STMMI_TEST_CUR_FILE  ${STMMI_TEST_SOURCES})

#message(STATUS "STMMI_TEST_CUR_FILE     ${STMMI_TEST_CUR_FILE}")
                file(RELATIVE_PATH  STMMI_TEST_CUR_REL_FILE  ${PROJECT_SOURCE_DIR}/test  ${STMMI_TEST_CUR_FILE})

                string(REGEX REPLACE "[./]" "_" STMMI_TEST_CUR_TGT ${STMMI_TEST_CUR_REL_FILE})

                add_executable(${STMMI_TEST_CUR_TGT} ${STMMI_TEST_CUR_FILE} ${STMMI_WITH_SOURCES} ${STMMI_EVS_SOURCES})
                #if (STMMI_WITH_SOURCES_LEN GREATER 0)
                #    target_link_libraries(${STMMI_TEST_CUR_TGT} stmmiwithsourcesobjlib) # link precompiled object files
                #endif (STMMI_WITH_SOURCES_LEN GREATER 0)

                if (NOT ("${STMMI_TARGET_LIB}" STREQUAL ""))
                    # This is needed for tests since the target lib is not installed yet
                    # and an #include<targetlib/header.h> would not be found otherwise
                    target_include_directories(${STMMI_TEST_CUR_TGT} BEFORE PRIVATE ${STMMI_INCLUDE_DIR})
                endif (NOT ("${STMMI_TARGET_LIB}" STREQUAL ""))
                if (STMMI_ADD_FAKES  OR  STMMI_ADD_EVS)
                    target_include_directories(${STMMI_TEST_CUR_TGT} BEFORE PRIVATE ${PROJECT_BINARY_DIR})
                endif (STMMI_ADD_FAKES  OR  STMMI_ADD_EVS)
                target_include_directories(${STMMI_TEST_CUR_TGT} BEFORE PRIVATE ${GTESTINCLUDEDIR})
                # tests can also involve non public part of the library!
                target_include_directories(${STMMI_TEST_CUR_TGT} BEFORE PRIVATE ${STMMI_SOURCES_DIR})
                target_include_directories(${STMMI_TEST_CUR_TGT} BEFORE PRIVATE ${STMMI_HEADERS_DIR})
                if (STMMI_FAKE_IFACE)
                    target_compile_definitions(${STMMI_TEST_CUR_TGT} PUBLIC STMI_TESTING_IFACE)
                endif (STMMI_FAKE_IFACE)
                if (STMMI_ADD_EVS)
                    target_compile_definitions(${STMMI_TEST_CUR_TGT} PUBLIC STMI_TESTING_ADD_EVS)
                endif (STMMI_ADD_EVS)

                target_link_libraries(${STMMI_TEST_CUR_TGT} ${STMMIFINDGTESTMAIN} ${STMMIFINDGTEST}) # link against gtest libs
                target_link_libraries(${STMMI_TEST_CUR_TGT}  ${STMMI_LINKED_LIBS})
#                foreach (STMMI_TEST_CUR_LIB  ${STMMI_LINKED_LIBS})
#message(STATUS "STMMI_TEST_CUR_LIB    ${STMMI_TEST_CUR_LIB}")
#                    target_link_libraries(${STMMI_TEST_CUR_TGT}  ${STMMI_TEST_CUR_LIB})
#                endforeach (STMMI_TEST_CUR_LIB  ${STMMI_LINKED_LIBS})

                add_test(NAME ${STMMI_TEST_CUR_TGT} COMMAND ${STMMI_TEST_CUR_TGT})       # this is how to add tests to CMake

            endforeach (STMMI_TEST_CUR_FILE  ${STMMI_TEST_SOURCES})
        endif (STMMI_DO_TESTING)
    endif (BUILD_TESTING)

endfunction(TestFiles STMMI_TEST_SOURCES  STMMI_WITH_SOURCES  STMMI_TARGET_LIB  STMMI_LINKED_LIBS  STMMI_ADD_FAKES  STMMI_ADD_EVS  STMMI_FAKE_IFACE)
