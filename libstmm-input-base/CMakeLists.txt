if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the stmm-input-base source code and call cmake from there")
endif()

string(STRIP "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(STMM_DEBUG)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(STMM_RELEASE)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
    set(STMM_MINSIZEREL)
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    set(STMM_RELWITHDEBINFO)
else()
    message(FATAL_ERROR "Wrong CMAKE_BUILD_TYPE, set to either Debug, Release, MinSizeRel or RelWithDebInfo")
endif()

cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../share/cmake) 


project(stmm-input-base CXX)

set(LIBRARY_OUTPUT_DIRECTORY "build")

#TODO remove: not used?
include(FindPkgConfig)
find_package(PkgConfig)

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_BASE_MAJOR_VERSION 0)
set(STMM_INPUT_BASE_MINOR_VERSION 1)
set(STMM_INPUT_BASE_VERSION "${STMM_INPUT_BASE_MAJOR_VERSION}.${STMM_INPUT_BASE_MINOR_VERSION}.0")
# required stmm-input version
set(STMM_INPUT_MAJOR_VERSION_REQ 0)
set(STMM_INPUT_MINOR_VERSION_REQ 1)
set(STMM_INPUT_VERSION_REQ "${STMM_INPUT_MAJOR_VERSION_REQ}.${STMM_INPUT_MINOR_VERSION_REQ}")

# Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
pkg_check_modules(STMMINPUT  REQUIRED  stmm-input>=${STMM_INPUT_VERSION_REQ})

list(APPEND STMMI_BASE_EXTRA_INCLUDE_DIRS  ${STMMINPUT_INCLUDE_DIRS})
list(APPEND STMMI_BASE_EXTRA_LIBS          ${STMMINPUT_LIBRARIES})

string(STRIP "${STMMI_BASE_EXTRA_LIBS}" STMMI_BASE_EXTRA_LIBS)
string(STRIP "${STMMI_BASE_EXTRA_INCLUDE_DIRS}" STMMI_BASE_EXTRA_INCLUDE_DIRS)

include_directories("${STMMI_BASE_EXTRA_INCLUDE_DIRS}")

# Headers dir
set(STMMI_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/include")
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-input-base")
# Header files
set(STMMI_HEADERS
        ${STMMI_HEADERS_DIR}/basicdevice.h
        ${STMMI_HEADERS_DIR}/basicdevicemanager.h
        ${STMMI_HEADERS_DIR}/childdevicemanager.h
        ${STMMI_HEADERS_DIR}/parentdevicemanager.h
        ${STMMI_HEADERS_DIR}/stmm-input-base.h
      )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
        ${STMMI_SOURCES_DIR}/callifsimplifier.cc
        ${STMMI_SOURCES_DIR}/callifsimplifier.h
        ${STMMI_SOURCES_DIR}/childdevicemanager.cc
        ${STMMI_SOURCES_DIR}/parentdevicemanager.cc
        #${STMMI_SOURCES_DIR}/basicdevice.cc
        ${STMMI_SOURCES_DIR}/basicdevicemanager.cc
        ${STMMI_SOURCES_DIR}/util.h
      )

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    set(STMMI_COMPILE_WARNINGS "-Wall -Wextra -Werror") # "-Wpedantic"
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(STMMI_COMPILE_WARNINGS "/W4 /WX /EHsc")
endif()
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${STMMI_COMPILE_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STMMI_COMPILE_WARNINGS}")

# Set install dir for pkg-config file
set(STMMI_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
# Create pkg-config file for shared library
configure_file("${PROJECT_SOURCE_DIR}/stmm-input-base.pc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-base.pc" @ONLY)

# Define library
add_library(stmm-input-base SHARED ${STMMI_SOURCES})

message(STATUS "")
message(STATUS "stmm-input-base was configured with the following options:")
message(STATUS " CMAKE_BUILD_TYPE:              ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:         ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_FLAGS:               ${CMAKE_CXX_FLAGS}")
message(STATUS " STMMI_BASE_EXTRA_INCLUDE_DIRS: ${STMMI_BASE_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMI_BASE_EXTRA_LIBS:         ${STMMI_BASE_EXTRA_LIBS}")
message(STATUS " STMMI_HEADERS:                 ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                 ${STMMI_SOURCES}")
message(STATUS " install prefix:                ${prefix}")
message(STATUS " BUILD_DOCS:                    ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                 ${BUILD_TESTING}")

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-input-base doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

# Set library versions
set_target_properties(stmm-input-base   PROPERTIES
        VERSION ${STMM_INPUT_BASE_VERSION}
        SOVERSION ${STMM_INPUT_BASE_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-base-${STMM_INPUT_BASE_VERSION}"
        )

# Determine header installation dir
target_include_directories(stmm-input-base   BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-base>  # <prefix>/include/stmm-input-base
)

target_link_libraries(stmm-input-base ${STMMI_BASE_EXTRA_LIBS}) # ${STMMI_BASE_EXTRA_LDFLAGS}

install(TARGETS stmm-input-base LIBRARY DESTINATION "lib")

install(FILES ${STMMI_HEADERS} DESTINATION "include/stmm-input-base")

install(FILES  ${PROJECT_BINARY_DIR}/libstmm-input-base.so   DESTINATION "lib")

install(FILES  ${PROJECT_BINARY_DIR}/stmm-input-base.pc   DESTINATION "lib/pkgconfig")

