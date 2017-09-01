# File: libstmm-input-dl/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

project(stmm-input-dl CXX)

set(LIBRARY_OUTPUT_DIRECTORY "build")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../share/cmake) 

include(CommonUtil)

CheckBinaryNotSourceTree()
CheckBuildType()
DefineSharedLibOption()
DefineCommonOptions()
DefineCommonCompileOptions()

include(FindPkgConfig)

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_DL_MAJOR_VERSION 0)
set(STMM_INPUT_DL_MINOR_VERSION 1)
set(STMM_INPUT_DL_VERSION "${STMM_INPUT_DL_MAJOR_VERSION}.${STMM_INPUT_DL_MINOR_VERSION}.0")

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

list(APPEND STMMI_DL_EXTRA_LIBS          "dl")

list(APPEND STMMI_DL_EXTRA_INCLUDE_DIRS  ${STMMINPUTBASE_INCLUDE_DIRS})
list(APPEND STMMI_DL_EXTRA_LIBS          ${STMMINPUTBASE_LIBRARIES})

string(STRIP "${STMMI_DL_EXTRA_LIBS}" STMMI_DL_EXTRA_LIBS)
string(STRIP "${STMMI_DL_EXTRA_INCLUDE_DIRS}" STMMI_DL_EXTRA_INCLUDE_DIRS)

include_directories("${STMMI_DL_EXTRA_INCLUDE_DIRS}")

# Headers dir
set(STMMI_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/include")
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-input-dl")
# Header files
set(STMMI_HEADERS
        ${STMMI_HEADERS_DIR}/pluginsdevicemanager.h
        ${STMMI_HEADERS_DIR}/stmm-input-dl-config.h
       )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
        ${STMMI_SOURCES_DIR}/pluginsdevicemanager.cc
       )

# Define library
add_library(stmm-input-dl ${STMMI_SOURCES} ${PROJECT_BINARY_DIR}/stmm-input-dl-config.cc)

DefineTargetPublicCompileOptions(stmm-input-dl)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(stmm-input-dl PUBLIC "-rdynamic")
endif()

include(GNUInstallDirs)
set(STMMI_PLUGINS_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/stmm-input-dl")
set(STMMI_PLUGINS_USER_DATA_DIR "$ENV{HOME}/.local/${CMAKE_INSTALL_DATADIR}/stmm-input-dl")

# Set version for stmm-input-base-config.cc.in
set(STMMI_PKG_VERSION "${STMM_INPUT_DL_VERSION}")
# Set plugin directories for stmm-input-base-config.cc.in
set(STMMI_PKG_DATA_DIR "${STMMI_PLUGINS_DATA_DIR}")
set(STMMI_PKG_USER_DATA_DIR "${STMMI_PLUGINS_USER_DATA_DIR}")
# Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-input-dl-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-dl-config.cc" @ONLY)

if (PKG_CONFIG_FOUND)
    # Set install dir for pkg-config file
    set(STMMI_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

    # Create pkg-config file for shared library
    configure_file("${PROJECT_SOURCE_DIR}/stmm-input-dl.pc.in"
                   "${PROJECT_BINARY_DIR}/stmm-input-dl.pc" @ONLY)
endif()

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input-dl was configured with the following options:")
message(STATUS " STMMI_HEADERS:                 ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                 ${STMMI_SOURCES}")
message(STATUS " STMMI_DL_EXTRA_INCLUDE_DIRS:   ${STMMI_DL_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMI_DL_EXTRA_LIBS:           ${STMMI_DL_EXTRA_LIBS}")
message(STATUS " CMAKE_BUILD_TYPE:              ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:         ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:    ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:               ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                ${CMAKE_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                    ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                 ${BUILD_TESTING}")
message(STATUS " STMMI_PLUGINS_DATA_DIR:        ${STMMI_PLUGINS_DATA_DIR}")
message(STATUS " STMMI_PLUGINS_USER_DATA_DIR:   ${STMMI_PLUGINS_USER_DATA_DIR}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-input-dl doc)
endif()

# Testing
add_subdirectory(googletest)
enable_testing()
add_subdirectory(test)

# Set library versions
set_target_properties(stmm-input-dl   PROPERTIES
        VERSION ${STMM_INPUT_DL_VERSION}
        SOVERSION ${STMM_INPUT_DL_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-dl-${STMM_INPUT_DL_VERSION}"
        )

# Determine header installation dir
target_include_directories(stmm-input-dl    BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-dl>  # <prefix>/include/stmm-input-dl
)

target_link_libraries(stmm-input-dl ${STMMI_DL_EXTRA_LIBS}) # ${STMMI_DL_EXTRA_LDFLAGS}

install(TARGETS stmm-input-dl LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS} DESTINATION "include/stmm-input-dl")

if (PKG_CONFIG_FOUND)
    install(FILES  ${PROJECT_BINARY_DIR}/stmm-input-dl.pc   DESTINATION "lib/pkgconfig")
endif()
