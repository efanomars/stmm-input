# File: libstmm-input-tst99/CMakeLists.txt

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_TST99_MAJOR_VERSION 0)
set(STMM_INPUT_TST99_MINOR_VERSION 1)
set(STMM_INPUT_TST99_VERSION "${STMM_INPUT_TST99_MAJOR_VERSION}.${STMM_INPUT_TST99_MINOR_VERSION}.0")
# required stmm-input version
set(STMM_INPUT_BASE_MAJOR_VERSION_REQ 0)
set(STMM_INPUT_BASE_MINOR_VERSION_REQ 1)
set(STMM_INPUT_BASE_VERSION_REQ "${STMM_INPUT_BASE_MAJOR_VERSION_REQ}.${STMM_INPUT_BASE_MINOR_VERSION_REQ}")

include(FindPkgConfig)

if (NOT PKG_CONFIG_FOUND)
     message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
endif()

pkg_check_modules(STMMINPUTBASE  REQUIRED  stmm-input-base>=${STMM_INPUT_BASE_VERSION_REQ})

list(APPEND STMMI_TST99_EXTRA_INCLUDE_DIRS  ${STMMINPUTBASE_INCLUDE_DIRS})
list(APPEND STMMI_TST99_EXTRA_LIBS          ${STMMINPUTBASE_LIBRARIES})

string(STRIP "${STMMI_TST99_EXTRA_LIBS}"         STMMI_TST99_EXTRA_LIBS)
string(STRIP "${STMMI_TST99_EXTRA_INCLUDE_DIRS}" STMMI_TST99_EXTRA_INCLUDE_DIRS)


# Headers dir
set(STMMI_TST99_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/test/libstmm-input-tst99/include")
set(STMMI_TST99_HEADERS_DIR  "${STMMI_TST99_INCLUDE_DIR}/stmm-input-tst99")
# Header files
set(STMMI_TST99_HEADERS
        ${STMMI_TST99_HEADERS_DIR}/libmain.h
      )
#
# Sources dir
set(STMMI_TST99_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test/libstmm-input-tst99/src")
# Source files (and headers only used for building)
set(STMMI_TST99_SOURCES
        ${STMMI_TST99_SOURCES_DIR}/libmain.cc
        ${STMMI_TST99_SOURCES_DIR}/tst99devicemanager.h
        ${STMMI_TST99_SOURCES_DIR}/tst99devicemanager.cc
        ${STMMI_TST99_SOURCES_DIR}/tst99capability.h
        ${STMMI_TST99_SOURCES_DIR}/tst99capability.cc
        ${STMMI_TST99_SOURCES_DIR}/tst99event.h
        ${STMMI_TST99_SOURCES_DIR}/tst99event.cc
        ${PROJECT_BINARY_DIR}/stddevicemanager.h
        ${PROJECT_BINARY_DIR}/stddevicemanager.cc
        ${PROJECT_BINARY_DIR}/devicemgmtevent.h
        ${PROJECT_BINARY_DIR}/devicemgmtevent.cc
      )

# Define library
add_library(stmm-input-tst99 SHARED ${STMMI_TST99_SOURCES})

target_include_directories(stmm-input-tst99 PUBLIC "${STMMI_TST99_EXTRA_INCLUDE_DIRS}")

target_include_directories(stmm-input-tst99 PUBLIC "${PROJECT_BINARY_DIR}")

DefineTargetPublicCompileOptions(stmm-input-tst99)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(stmm-input-tst99 PUBLIC "-Wl,-E")
endif()

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input-tst99 was configured with the following options:")
message(STATUS " STMMI_TST99_HEADERS:             ${STMMI_TST99_HEADERS}")
message(STATUS " STMMI_TST99_SOURCES:             ${STMMI_TST99_SOURCES}")
message(STATUS " STMMI_TST99_EXTRA_INCLUDE_DIRS:  ${STMMI_TST99_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMI_TST99_EXTRA_LIBS:          ${STMMI_TST99_EXTRA_LIBS}")
message(STATUS " CMAKE_BUILD_TYPE:                ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_LINK_EXECUTABLE        ${CMAKE_CXX_LINK_EXECUTABLE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:           ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:      ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                 ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                  ${CMAKE_INSTALL_PREFIX}")
endif()

# Set library versions
set_target_properties(stmm-input-tst99   PROPERTIES
        VERSION ${STMM_INPUT_TST99_VERSION}
        SOVERSION ${STMM_INPUT_TST99_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-tst99-${STMM_INPUT_TST99_VERSION}"
        )

# Determine header installation dir
target_include_directories(stmm-input-tst99   BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_TST99_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-tst99>  # <prefix>/include/stmm-input-tst99
)

target_link_libraries(stmm-input-tst99 ${STMMI_TST99_EXTRA_LIBS}) # ${STMMI_TST99_EXTRA_LDFLAGS}

