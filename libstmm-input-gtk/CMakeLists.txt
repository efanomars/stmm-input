# File: libstmm-input-gtk/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

option(OMIT_X11 "Exclude X11 (XI2) related device managers" OFF)
mark_as_advanced(OMIT_X11)

option(OMIT_PLUGINS "Exclude plugins device manager (stmm-input-dl)" OFF)
mark_as_advanced(OMIT_PLUGINS)

project(stmm-input-gtk CXX)

set(LIBRARY_OUTPUT_DIRECTORY "build")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../share/cmake) 

include(CommonUtil)

CheckBinaryNotSourceTree()
CheckBuildType()
DefineSharedLibOption()
DefineCommonOptions()
DefineCommonCompileOptions()

include(FindPkgConfig)
if (NOT PKG_CONFIG_FOUND)
     message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
endif()

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_GTK_MAJOR_VERSION 0)
set(STMM_INPUT_GTK_MINOR_VERSION 1)
set(STMM_INPUT_GTK_VERSION "${STMM_INPUT_GTK_MAJOR_VERSION}.${STMM_INPUT_GTK_MINOR_VERSION}.0")

# required stmm-input-ev version
set(STMM_INPUT_MAJOR_VERSION_REQ 0)
set(STMM_INPUT_MINOR_VERSION_REQ 1)
set(STMM_INPUT_VERSION_REQ "${STMM_INPUT_MAJOR_VERSION_REQ}.${STMM_INPUT_MINOR_VERSION_REQ}")

set(GTKMM_VERSION_REQ "3.14.0")

# Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
pkg_check_modules(STMMINPUT      REQUIRED  stmm-input>=${STMM_INPUT_VERSION_REQ})
pkg_check_modules(GTKMM  REQUIRED  gtkmm-3.0>=${GTKMM_VERSION_REQ})

list(APPEND STMMI_GTK_EXTRA_INCLUDE_DIRS
    ${STMMINPUT_INCLUDE_DIRS}
    ${GTKMM_INCLUDE_DIRS}
    )
list(APPEND STMMI_GTK_EXTRA_LIBS
    ${STMMINPUT_LIBRARIES}
    ${GTKMM_LIBRARIES}
    )

string(STRIP "${STMMI_GTK_EXTRA_LIBS}" STMMI_GTK_EXTRA_LIBS)
string(STRIP "${STMMI_GTK_EXTRA_INCLUDE_DIRS}" STMMI_GTK_EXTRA_INCLUDE_DIRS)

include_directories("${STMMI_GTK_EXTRA_INCLUDE_DIRS}")

# Headers dir
set(STMMI_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/include")
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-input-gtk")
# Header files
# "SubProject" STMMI_HEADERS_DATA depends on
#   libstmm-input
#   gtkmm-3.0-dev
set(STMMI_HEADERS
        ${STMMI_HEADERS_DIR}/gtkaccessor.h
        ${STMMI_HEADERS_DIR}/stmm-input-gtk-config.h
        ${STMMI_HEADERS_DIR}/stmm-input-gtk.h
        )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES
        ${STMMI_SOURCES_DIR}/gtkaccessor.cc
        )

# Define library
add_library(stmm-input-gtk ${STMMI_SOURCES} ${PROJECT_BINARY_DIR}/stmm-input-gtk-config.cc)

DefineTargetPublicCompileOptions(stmm-input-gtk)

# Set version for stmm-input-gtk-config.cc.in
set(STMMI_PKG_VERSION "${STMM_INPUT_GTK_VERSION}")
# Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-input-gtk-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-gtk-config.cc" @ONLY)

# Set install dir for pkg-config file
set(STMMI_GTK_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
# Create pkg-config file for shared library
configure_file("${PROJECT_SOURCE_DIR}/stmm-input-gtk.pc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-gtk.pc" @ONLY)

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input-gtk was configured with the following options:")
message(STATUS " STMMI_HEADERS:                 ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                 ${STMMI_SOURCES}")
message(STATUS " STMMI_GTK_EXTRA_INCLUDE_DIRS:  ${STMMI_GTK_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMI_GTK_EXTRA_LIBS:          ${STMMI_GTK_EXTRA_LIBS}")
message(STATUS " CMAKE_BUILD_TYPE:              ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:         ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:    ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:               ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                ${STMMI_GTK_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                    ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                 ${BUILD_TESTING}")
message(STATUS " OMIT_X11:                      ${OMIT_X11}")
message(STATUS " OMIT_PLUGINS:                  ${OMIT_PLUGINS}")
endif()

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-input-gtk doc)
endif()

# Testing
add_subdirectory(googletest)
enable_testing()
add_subdirectory(test)

# Set library versions
set_target_properties(stmm-input-gtk PROPERTIES
        VERSION ${STMM_INPUT_GTK_VERSION}
        SOVERSION ${STMM_INPUT_GTK_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-gtk-${STMM_INPUT_GTK_VERSION}"
        )

# Determine header installation dir
target_include_directories(stmm-input-gtk    BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-gtk>  # <prefix>/include/stmm-input-gtk
)

target_link_libraries(stmm-input-gtk ${STMMI_GTK_EXTRA_LIBS}) # ${STMMI_GTK_EXTRA_LDFLAGS}

install(TARGETS stmm-input-gtk LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS}   DESTINATION "include/stmm-input-gtk")

install(FILES  ${PROJECT_BINARY_DIR}/stmm-input-gtk.pc   DESTINATION "lib/pkgconfig")
