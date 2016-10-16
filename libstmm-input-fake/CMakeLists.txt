# File: libstmm-input-fake/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(stmm-input-fake CXX)

set(LIBRARY_OUTPUT_DIRECTORY "build")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../share/cmake) 

include(CommonUtil)

CheckBinaryNotSourceTree()
CheckBuildType()
#header-only# DefineSharedLibOption()

include(FindPkgConfig)

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_FAKE_MAJOR_VERSION 0)
set(STMM_INPUT_FAKE_MINOR_VERSION 1)
set(STMM_INPUT_FAKE_VERSION "${STMM_INPUT_FAKE_MAJOR_VERSION}.${STMM_INPUT_FAKE_MINOR_VERSION}.0")

# required stmm-input-ev version
set(STMM_INPUT_EV_MAJOR_VERSION_REQ 0)
set(STMM_INPUT_EV_MINOR_VERSION_REQ 1)
set(STMM_INPUT_EV_VERSION_REQ "${STMM_INPUT_EV_MAJOR_VERSION_REQ}.${STMM_INPUT_EV_MINOR_VERSION_REQ}")

if (PKG_CONFIG_FOUND)
    pkg_check_modules(STMMINPUTEV  REQUIRED  stmm-input-ev>=${STMM_INPUT_EV_VERSION_REQ})
else (PKG_CONFIG_FOUND)
    list(APPEND STMMINPUTEV_INCLUDE_DIRS
            "${PROJECT_SOURCE_DIR}/../libstmm-input/include"
            "${PROJECT_SOURCE_DIR}/../libstmm-input-base/include"
            "${PROJECT_SOURCE_DIR}/../libstmm-input-ev/include")
    list(APPEND STMMINPUTEV_LIBRARIES
            "${PROJECT_SOURCE_DIR}/../libstmm-input/build/libstmm-input.a"
            "${PROJECT_SOURCE_DIR}/../libstmm-input-base/build/libstmm-input-base.a"
            "${PROJECT_SOURCE_DIR}/../libstmm-input-ev/build/libstmm-input-ev.a")
endif()

list(APPEND STMMI_FAKE_EXTRA_INCLUDE_DIRS  ${STMMINPUTEV_INCLUDE_DIRS})
list(APPEND STMMI_FAKE_EXTRA_LIBS          ${STMMINPUTEV_LIBRARIES})

string(STRIP "${STMMI_FAKE_EXTRA_LIBS}" STMMI_FAKE_EXTRA_LIBS)
string(STRIP "${STMMI_FAKE_EXTRA_INCLUDE_DIRS}" STMMI_FAKE_EXTRA_INCLUDE_DIRS)

include_directories("${STMMI_FAKE_EXTRA_INCLUDE_DIRS}")

# Headers dir
set(STMMI_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/include")
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-input-fake")
# Header files
# depends on
#   libstmm-input
#   libstmm-input-base
#   libstmm-input-ev
set(STMMI_HEADERS_FAKES
        ${STMMI_HEADERS_DIR}/fakedevice.h
        ${STMMI_HEADERS_DIR}/fakedevicemanager.h
        ${STMMI_HEADERS_DIR}/fakejoystickdevice.h
        ${STMMI_HEADERS_DIR}/fakekeydevice.h
        ${STMMI_HEADERS_DIR}/fakepointerdevice.h
        ${STMMI_HEADERS_DIR}/faketouchdevice.h
        ${STMMI_HEADERS_DIR}/stmm-input-fake.h
        )
# Concatenate header file lists
#set(STMMI_HEADERS)
list(APPEND STMMI_HEADERS
        ${STMMI_HEADERS_FAKES}
        )
#
#header-only# # Sources dir
#header-only# set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
#header-only# # Source files (and headers only used for building)
#header-only# # Concatenate source file lists
#header-only# list(APPEND  STMMI_SOURCES
#header-only#         )

DefineCallerCompileOptions()

# Set install dir for pkg-config file
set(STMMI_FAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

if (PKG_CONFIG_FOUND)
    # Create pkg-config file for shared library
    configure_file("${PROJECT_SOURCE_DIR}/stmm-input-fake.pc.in"
                   "${PROJECT_BINARY_DIR}/stmm-input-fake.pc" @ONLY)
endif()

# Define library
add_library(stmm-input-fake INTERFACE ) # ${STMMI_SOURCES}

message(STATUS "")
message(STATUS "stmm-input-fake was configured with the following options:")
message(STATUS " CMAKE_BUILD_TYPE:              ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:         ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_FLAGS:               ${CMAKE_CXX_FLAGS}")
message(STATUS " STMMI_FAKE_EXTRA_INCLUDE_DIRS: ${STMMI_FAKE_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMI_FAKE_EXTRA_LIBS:         ${STMMI_FAKE_EXTRA_LIBS}")
message(STATUS " STMMI_HEADERS:                 ${STMMI_HEADERS}")
#header-only# message(STATUS " STMMI_SOURCES:                 ${STMMI_SOURCES}")
message(STATUS " install prefix:                ${prefix}")
message(STATUS " BUILD_DOCS:                    ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                 ${BUILD_TESTING}")

# Documentation
add_subdirectory(doc)
#header-only# if (BUILD_DOCS)
#header-only#     # build automatically, otherwise call `make doc`
#header-only#     add_dependencies(stmm-input-fake doc)
#header-only# endif()

# Testing
enable_testing()
add_subdirectory(test)

#header-only# # Set library versions
#header-only# set_target_properties(stmm-input-fake PROPERTIES
#header-only#         VERSION ${STMM_INPUT_FAKE_VERSION}
#header-only#         SOVERSION ${STMM_INPUT_FAKE_MAJOR_VERSION}
#header-only# #        OUTPUT_NAME "stmm-input-fake-${STMM_INPUT_FAKE_VERSION}"
#header-only#         )

#header-only# # Determine header installation dir
#header-only# target_include_directories(stmm-input-fake    BEFORE INTERFACE
#header-only#            $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
#header-only#            $<INSTALL_INTERFACE:include/stmm-input-fake>  # <prefix>/include/stmm-input-fake
#header-only#           )

#header-only# target_link_libraries(stmm-input-fake INTERFACE ${STMMI_FAKE_EXTRA_LIBS}) # ${STMMI_FAKE_EXTRA_LDFLAGS}

#header-only# install(TARGETS stmm-input-fake LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS}   DESTINATION "include/stmm-input-fake")

if (PKG_CONFIG_FOUND)
    install(FILES  ${PROJECT_BINARY_DIR}/stmm-input-fake.pc   DESTINATION "lib/pkgconfig")
endif()
