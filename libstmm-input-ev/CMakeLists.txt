# File: libstmm-input-ev/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(stmm-input-ev CXX)

set(LIBRARY_OUTPUT_DIRECTORY "build")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../share/cmake) 

include(CommonUtil)

CheckBinaryNotSourceTree()
CheckBuildType()
DefineSharedLibOption()
DefineCommonOptions()

include(FindPkgConfig)

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_EV_MAJOR_VERSION 0)
set(STMM_INPUT_EV_MINOR_VERSION 1)
set(STMM_INPUT_EV_VERSION "${STMM_INPUT_EV_MAJOR_VERSION}.${STMM_INPUT_EV_MINOR_VERSION}.0")

# required stmm-input-base version
set(STMM_INPUT_BASE_MAJOR_VERSION_REQ 0)
set(STMM_INPUT_BASE_MINOR_VERSION_REQ 1)
set(STMM_INPUT_BASE_VERSION_REQ "${STMM_INPUT_BASE_MAJOR_VERSION_REQ}.${STMM_INPUT_BASE_MINOR_VERSION_REQ}")

if (PKG_CONFIG_FOUND)
    # Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
    pkg_check_modules(STMMINPUTBASE  REQUIRED  stmm-input-base>=${STMM_INPUT_BASE_VERSION_REQ})
else (PKG_CONFIG_FOUND)
    list(APPEND STMMINPUTBASE_INCLUDE_DIRS
            "${PROJECT_SOURCE_DIR}/../libstmm-input/include"
            "${PROJECT_SOURCE_DIR}/../libstmm-input-base/include")
    list(APPEND STMMINPUTBASE_LIBRARIES
            "${PROJECT_SOURCE_DIR}/../libstmm-input/build/libstmm-input.a"
            "${PROJECT_SOURCE_DIR}/../libstmm-input-base/build/libstmm-input-base.a")
endif()

list(APPEND STMMI_EV_EXTRA_INCLUDE_DIRS  ${STMMINPUTBASE_INCLUDE_DIRS})
list(APPEND STMMI_EV_EXTRA_LIBS          ${STMMINPUTBASE_LIBRARIES})

string(STRIP "${STMMI_EV_EXTRA_LIBS}" STMMI_EV_EXTRA_LIBS)
string(STRIP "${STMMI_EV_EXTRA_INCLUDE_DIRS}" STMMI_EV_EXTRA_INCLUDE_DIRS)

include_directories("${STMMI_EV_EXTRA_INCLUDE_DIRS}")

# Headers dir
set(STMMI_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/include")
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-input-ev")
# Header files
set(STMMI_HEADERS
        ${STMMI_HEADERS_DIR}/devicemgmtcapability.h
        ${STMMI_HEADERS_DIR}/devicemgmtevent.h
        ${STMMI_HEADERS_DIR}/joystickcapability.h
        ${STMMI_HEADERS_DIR}/joystickevent.h
        ${STMMI_HEADERS_DIR}/keycapability.h
        ${STMMI_HEADERS_DIR}/keyevent.h
        ${STMMI_HEADERS_DIR}/pointercapability.h
        ${STMMI_HEADERS_DIR}/pointerevent.h
        ${STMMI_HEADERS_DIR}/stddevicemanager.h
        ${STMMI_HEADERS_DIR}/stdparentdevicemanager.h
        ${STMMI_HEADERS_DIR}/stmm-input-ev.h
        ${STMMI_HEADERS_DIR}/touchcapability.h
        ${STMMI_HEADERS_DIR}/touchevent.h
       )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
        ${STMMI_SOURCES_DIR}/devicemgmtcapability.cc
        ${STMMI_SOURCES_DIR}/devicemgmtevent.cc
        ${STMMI_SOURCES_DIR}/joystickcapability.cc
        ${STMMI_SOURCES_DIR}/joystickevent.cc
        ${STMMI_SOURCES_DIR}/keycapability.cc
        ${STMMI_SOURCES_DIR}/keyevent.cc
        ${STMMI_SOURCES_DIR}/pointercapability.cc
        ${STMMI_SOURCES_DIR}/pointerevent.cc
        ${STMMI_SOURCES_DIR}/stddevicemanager.cc
        ${STMMI_SOURCES_DIR}/stdparentdevicemanager.cc
        ${STMMI_SOURCES_DIR}/touchcapability.cc
        ${STMMI_SOURCES_DIR}/touchevent.cc
       )

DefineCallerCompileOptions()

# Set install dir for pkg-config file
set(STMMI_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

if (PKG_CONFIG_FOUND)
    # Create pkg-config file for shared library
    configure_file("${PROJECT_SOURCE_DIR}/stmm-input-ev.pc.in"
                   "${PROJECT_BINARY_DIR}/stmm-input-ev.pc" @ONLY)
endif()

# Define library
add_library(stmm-input-ev ${STMMI_SOURCES})

message(STATUS "")
message(STATUS "stmm-input-ev was configured with the following options:")
message(STATUS " CMAKE_BUILD_TYPE:              ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:         ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:    ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:               ${CMAKE_CXX_FLAGS}")
message(STATUS " STMMI_EV_EXTRA_INCLUDE_DIRS:   ${STMMI_EV_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMI_EV_EXTRA_LIBS:           ${STMMI_EV_EXTRA_LIBS}")
message(STATUS " STMMI_HEADERS:                 ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                 ${STMMI_SOURCES}")
message(STATUS " install prefix:                ${prefix}")
message(STATUS " BUILD_DOCS:                    ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                 ${BUILD_TESTING}")

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-input-ev doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

# Set library versions
set_target_properties(stmm-input-ev   PROPERTIES
        VERSION ${STMM_INPUT_EV_VERSION}
        SOVERSION ${STMM_INPUT_EV_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-ev-${STMM_INPUT_EV_VERSION}"
        )

# Determine header installation dir
target_include_directories(stmm-input-ev    BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-ev>  # <prefix>/include/stmm-input-ev
)

target_link_libraries(stmm-input-ev ${STMMI_EV_EXTRA_LIBS}) # ${STMMI_EV_EXTRA_LDFLAGS}

install(TARGETS stmm-input-ev LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS} DESTINATION "include/stmm-input-ev")

if (PKG_CONFIG_FOUND)
    install(FILES  ${PROJECT_BINARY_DIR}/stmm-input-ev.pc   DESTINATION "lib/pkgconfig")
endif()
