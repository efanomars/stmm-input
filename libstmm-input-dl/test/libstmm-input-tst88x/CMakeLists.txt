# File: libstmm-input-tst88x/CMakeLists.txt

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_TST88X_MAJOR_VERSION 0)
set(STMM_INPUT_TST88X_MINOR_VERSION 1)
set(STMM_INPUT_TST88X_VERSION "${STMM_INPUT_TST88X_MAJOR_VERSION}.${STMM_INPUT_TST88X_MINOR_VERSION}.0")

# Headers dir
set(STMMI_TST88X_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/test/libstmm-input-tst88x/include")
set(STMMI_TST88X_HEADERS_DIR  "${STMMI_TST88X_INCLUDE_DIR}/stmm-input-tst88x")
# Header files
set(STMMI_TST88X_HEADERS
        "${STMMI_TST88X_HEADERS_DIR}/libmain.h"
      )
#
# Sources dir
set(STMMI_TST88X_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test/libstmm-input-tst88x/src")
# Source files (and headers only used for building)
set(STMMI_TST88X_SOURCES
        "${STMMI_TST88X_SOURCES_DIR}/libmain.cc"
        "${STMMI_TST88X_SOURCES_DIR}/tst88xdevicemanager.h"
        "${STMMI_TST88X_SOURCES_DIR}/tst88xdevicemanager.cc"
        "${PROJECT_BINARY_DIR}/keycapability.h"
        "${PROJECT_BINARY_DIR}/keycapability.cc"
        "${PROJECT_BINARY_DIR}/keyevent.h"
        "${PROJECT_BINARY_DIR}/keyevent.cc"
        "${PROJECT_BINARY_DIR}/stddevicemanager.h"
        "${PROJECT_BINARY_DIR}/stddevicemanager.cc"
        "${PROJECT_BINARY_DIR}/devicemgmtevent.h"
        "${PROJECT_BINARY_DIR}/devicemgmtevent.cc"
      )

# Define library
add_library(stmm-input-tst88x SHARED ${STMMI_TST88X_SOURCES})

target_include_directories(stmm-input-tst88x PUBLIC "${STMMI_TST88X_EXTRA_INCLUDE_DIRS}")

target_include_directories(stmm-input-tst88x PUBLIC "${PROJECT_BINARY_DIR}")

target_link_libraries(stmm-input-tst88x stmm-input-base)
target_link_libraries(stmm-input-tst88x ${STMMI_TST88X_EXTRA_LIBS})

target_link_libraries(stmm-input-tst88x stmm-input-tst88a)

target_include_directories(stmm-input-tst88x   BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_TST88X_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-tst88x>  # <prefix>/include/stmm-input-tst88x
)

set_target_properties(stmm-input-tst88x   PROPERTIES
        VERSION ${STMM_INPUT_TST88X_VERSION}
        SOVERSION ${STMM_INPUT_TST88X_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-tst88x-${STMM_INPUT_TST88X_VERSION}"
        )
set_target_properties(stmm-input-tst88x   PROPERTIES
        DEBUG_POSTFIX ""
        RELEASE_POSTFIX ""
        )

DefineTargetPublicCompileOptions(stmm-input-tst88x)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(stmm-input-tst88x PUBLIC "-Wl,-E")
endif()

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input-tst88x was configured with the following options:")
message(STATUS " STMMI_TST88X_HEADERS:            ${STMMI_TST88X_HEADERS}")
message(STATUS " STMMI_TST88X_SOURCES:            ${STMMI_TST88X_SOURCES}")
message(STATUS " STMMI_TST88X_EXTRA_INCLUDE_DIRS: ${STMMI_TST88X_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMI_TST88X_EXTRA_LIBS:         ${STMMI_TST88X_EXTRA_LIBS}")
message(STATUS " CMAKE_BUILD_TYPE:                ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_LINK_EXECUTABLE        ${CMAKE_CXX_LINK_EXECUTABLE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:           ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:      ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                 ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                  ${CMAKE_INSTALL_PREFIX}")
endif()
