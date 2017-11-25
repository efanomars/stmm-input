# File: libstmm-input-tst88x/CMakeLists.txt

# cmake_minimum_required(VERSION 3.0)
# 
# project(stmm-input-tst88x CXX)
# 
# set(LIBRARY_OUTPUT_DIRECTORY "build")
# 
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../share/cmake) 
# 
# include(CommonUtil)
# 
# CheckBinaryNotSourceTree()
# CheckBuildType()
# DefineSharedLibOption()
# DefineCommonOptions()
# DefineCommonCompileOptions()

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_TST88X_MAJOR_VERSION 0)
set(STMM_INPUT_TST88X_MINOR_VERSION 1)
set(STMM_INPUT_TST88X_VERSION "${STMM_INPUT_TST88X_MAJOR_VERSION}.${STMM_INPUT_TST88X_MINOR_VERSION}.0")
# required stmm-input version
set(STMM_INPUT_BASE_MAJOR_VERSION_REQ 0)
set(STMM_INPUT_BASE_MINOR_VERSION_REQ 1)
set(STMM_INPUT_BASE_VERSION_REQ "${STMM_INPUT_BASE_MAJOR_VERSION_REQ}.${STMM_INPUT_BASE_MINOR_VERSION_REQ}")

include(FindPkgConfig)

if (NOT PKG_CONFIG_FOUND)
     message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
endif()
# if (PKG_CONFIG_FOUND)
#     # Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
#     pkg_check_modules(STMMINPUT  REQUIRED  stmm-input>=${STMM_INPUT_VERSION_REQ})
# else (PKG_CONFIG_FOUND)
#     list(APPEND STMMINPUT_INCLUDE_DIRS
#             "${PROJECT_SOURCE_DIR}/../libstmm-input/include")
#     list(APPEND STMMINPUT_LIBRARIES
#             "${PROJECT_SOURCE_DIR}/../libstmm-input/build/libstmm-input.a")
# endif()

pkg_check_modules(STMMINPUTBASE  REQUIRED  stmm-input-base>=${STMM_INPUT_BASE_VERSION_REQ})

list(APPEND STMMI_TST88X_EXTRA_INCLUDE_DIRS  ${STMMINPUTBASE_INCLUDE_DIRS})
list(APPEND STMMI_TST88X_EXTRA_LIBS          ${STMMINPUTBASE_LIBRARIES})

string(STRIP "${STMMI_TST88X_EXTRA_LIBS}"         STMMI_TST88X_EXTRA_LIBS)
string(STRIP "${STMMI_TST88X_EXTRA_INCLUDE_DIRS}" STMMI_TST88X_EXTRA_INCLUDE_DIRS)


# Headers dir
set(STMMI_TST88X_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/test/libstmm-input-tst88x/include")
set(STMMI_TST88X_HEADERS_DIR  "${STMMI_TST88X_INCLUDE_DIR}/stmm-input-tst88x")
# Header files
set(STMMI_TST88X_HEADERS
        ${STMMI_TST88X_HEADERS_DIR}/libmain.h
      )
#
# Sources dir
set(STMMI_TST88X_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test/libstmm-input-tst88x/src")
# Source files (and headers only used for building)
set(STMMI_TST88X_SOURCES
        ${STMMI_TST88X_SOURCES_DIR}/libmain.cc
        ${STMMI_TST88X_SOURCES_DIR}/tst88xdevicemanager.h
        ${STMMI_TST88X_SOURCES_DIR}/tst88xdevicemanager.cc
        ${PROJECT_BINARY_DIR}/keycapability.h
        ${PROJECT_BINARY_DIR}/keycapability.cc
        ${PROJECT_BINARY_DIR}/keyevent.h
        ${PROJECT_BINARY_DIR}/keyevent.cc
        ${PROJECT_BINARY_DIR}/stddevicemanager.h
        ${PROJECT_BINARY_DIR}/stddevicemanager.cc
        ${PROJECT_BINARY_DIR}/devicemgmtevent.h
        ${PROJECT_BINARY_DIR}/devicemgmtevent.cc
      )

# Define library
add_library(stmm-input-tst88x SHARED ${STMMI_TST88X_SOURCES})

target_include_directories(stmm-input-tst88x PUBLIC "${STMMI_TST88X_EXTRA_INCLUDE_DIRS}")

target_include_directories(stmm-input-tst88x PUBLIC "${PROJECT_BINARY_DIR}")

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

# Set library versions
set_target_properties(stmm-input-tst88x   PROPERTIES
        VERSION ${STMM_INPUT_TST88X_VERSION}
        SOVERSION ${STMM_INPUT_TST88X_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-tst88x-${STMM_INPUT_TST88X_VERSION}"
        )
set_target_properties(stmm-input-tst88x   PROPERTIES
        DEBUG_POSTFIX ""
        RELEASE_POSTFIX ""
        )

# Determine header installation dir
target_include_directories(stmm-input-tst88x   BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_TST88X_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-tst88x>  # <prefix>/include/stmm-input-tst88x
)

target_link_libraries(stmm-input-tst88x ${STMMI_TST88X_EXTRA_LIBS}) # ${STMMI_TST88X_EXTRA_LDFLAGS}

target_link_libraries(stmm-input-tst88x stmm-input-tst88a)
