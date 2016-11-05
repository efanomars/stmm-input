# File: libstmm-input-base/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(stmm-input-base CXX)

set(LIBRARY_OUTPUT_DIRECTORY "build")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../share/cmake) 

include(CommonUtil)

CheckBinaryNotSourceTree()
CheckBuildType()
DefineSharedLibOption()

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

include(FindPkgConfig)

if (PKG_CONFIG_FOUND)
    # Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
    pkg_check_modules(STMMINPUT  REQUIRED  stmm-input>=${STMM_INPUT_VERSION_REQ})
else (PKG_CONFIG_FOUND)
    list(APPEND STMMINPUT_INCLUDE_DIRS
            "${PROJECT_SOURCE_DIR}/../libstmm-input/include")
    list(APPEND STMMINPUT_LIBRARIES
            "${PROJECT_SOURCE_DIR}/../libstmm-input/build/libstmm-input.a")
endif()

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

DefineCallerCompileOptions()

# Set install dir for pkg-config file
set(STMMI_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

if (PKG_CONFIG_FOUND)
    # Create pkg-config file for shared library
    configure_file("${PROJECT_SOURCE_DIR}/stmm-input-base.pc.in"
                   "${PROJECT_BINARY_DIR}/stmm-input-base.pc" @ONLY)
endif()

# Define library
add_library(stmm-input-base ${STMMI_SOURCES})

message(STATUS "")
message(STATUS "stmm-input-base was configured with the following options:")
message(STATUS " CMAKE_BUILD_TYPE:              ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:         ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:    ${CMAKE_CXX_COMPILER_VERSION}")
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

install(TARGETS stmm-input-base LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS} DESTINATION "include/stmm-input-base")

if (PKG_CONFIG_FOUND)
    install(FILES  ${PROJECT_BINARY_DIR}/stmm-input-base.pc   DESTINATION "lib/pkgconfig")
endif()

