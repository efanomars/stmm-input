if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the stmm-input-gtk source code and call cmake from there")
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


project(stmm-input-gtk CXX)

set(LIBRARY_OUTPUT_DIRECTORY "build")

include(FindPkgConfig)
find_package(PkgConfig REQUIRED)

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_GTK_MAJOR_VERSION 0)
set(STMM_INPUT_GTK_MINOR_VERSION 1)
set(STMM_INPUT_GTK_VERSION "${STMM_INPUT_GTK_MAJOR_VERSION}.${STMM_INPUT_GTK_MINOR_VERSION}.0")
# required stmm-input version
set(STMM_INPUT_MAJOR_VERSION_REQ 0)
set(STMM_INPUT_MINOR_VERSION_REQ 1)
set(STMM_INPUT_VERSION_REQ "${STMM_INPUT_MAJOR_VERSION_REQ}.${STMM_INPUT_MINOR_VERSION_REQ}")
# required stmm-input-base version
set(STMM_INPUT_BASE_MAJOR_VERSION_REQ 0)
set(STMM_INPUT_BASE_MINOR_VERSION_REQ 1)
set(STMM_INPUT_BASE_VERSION_REQ "${STMM_INPUT_BASE_MAJOR_VERSION_REQ}.${STMM_INPUT_BASE_MINOR_VERSION_REQ}")
# required stmm-input-ev version
set(STMM_INPUT_EV_MAJOR_VERSION_REQ 0)
set(STMM_INPUT_EV_MINOR_VERSION_REQ 1)
set(STMM_INPUT_EV_VERSION_REQ "${STMM_INPUT_EV_MAJOR_VERSION_REQ}.${STMM_INPUT_EV_MINOR_VERSION_REQ}")

set(GTKMM_VERSION_REQ "3.4.2")
set(XI_VERSION_REQ "1.7.4")

# Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
pkg_check_modules(GTKMM  REQUIRED  gtkmm-3.0>=${GTKMM_VERSION_REQ})
pkg_check_modules(XI     REQUIRED  xi>=${XI_VERSION_REQ})
pkg_check_modules(STMMINPUTBASE  REQUIRED  stmm-input-base>=${STMM_INPUT_BASE_VERSION_REQ})
pkg_check_modules(STMMINPUTEV    REQUIRED  stmm-input-ev>=${STMM_INPUT_EV_VERSION_REQ})
pkg_check_modules(STMMINPUT      REQUIRED  stmm-input>=${STMM_INPUT_VERSION_REQ})

#Debug execute_process(
#Debug         COMMAND pkg-config --atleast-version 0.2 stmm-input
#Debug         RESULT_VARIABLE TEST_O_TEST)
#Debug message(STATUS "${TEST_O_TEST}")
#Debug message(STATUS "${STMMINPUT_FOUND}")
#Debug message(STATUS "${STMMINPUT_VERSION}")

list(APPEND STMMI_GTK_EXTRA_INCLUDE_DIRS
    ${STMMINPUT_INCLUDE_DIRS}
    ${STMMINPUTBASE_INCLUDE_DIRS}
    ${STMMINPUTEV_INCLUDE_DIRS}
    ${GTKMM_INCLUDE_DIRS}
    ${XI_INCLUDE_DIRS}
    )
list(APPEND STMMI_GTK_EXTRA_LIBS
    ${STMMINPUT_LIBRARIES}
    ${STMMINPUTBASE_LIBRARIES}
    ${STMMINPUTEV_LIBRARIES}
    ${GTKMM_LIBRARIES}
    ${XI_LIBRARIES}
    )

string(STRIP "${STMMI_GTK_EXTRA_LIBS}" STMMI_GTK_EXTRA_LIBS)
string(STRIP "${STMMI_GTK_EXTRA_INCLUDE_DIRS}" STMMI_GTK_EXTRA_INCLUDE_DIRS)

include_directories("${STMMI_GTK_EXTRA_INCLUDE_DIRS}")

# Headers dir
set(STMMI_GTK_HEADERS_DIR  "${PROJECT_SOURCE_DIR}/include/stmm-input-gtk")
# Header files
# "SubProject" STMMI_GTK_HEADERS_DATA depends on
#   libstmm-input
#   gtkmm-3.0-dev
set(STMMI_GTK_HEADERS_DATA
        ${STMMI_GTK_HEADERS_DIR}/gtkaccessor.h
        ${STMMI_GTK_HEADERS_DIR}/gdkkeyconverter.h
        ${STMMI_GTK_HEADERS_DIR}/gdkkeyconverterevdev.h
        ${STMMI_GTK_HEADERS_DIR}/keyrepeatmode.h
        )
# "SubProject" STMMI_GTK_HEADERS_MAS depends on 
#   STMMI_GTK_HEADERS_DATA 
#   libstmm-input-base
set(STMMI_GTK_HEADERS_MAS
# Moved to sources        ${STMMI_GTK_HEADERS_DIR}/masgtkdevicemanager.h
        )
# "SubProject" STMMI_GTK_HEADERS_FLO depends on
#   STMMI_GTK_HEADERS_DATA 
#   libstmm-input-base
#   libxi-dev
set(STMMI_GTK_HEADERS_FLO
# Moved to sources        ${STMMI_GTK_HEADERS_DIR}/flogtkdevicemanager.h
        )
# "SubProject" STMMI_GTK_HEADERS_JS depends on
#   STMMI_GTK_HEADERS_DATA 
#   libstmm-input-base
set(STMMI_GTK_HEADERS_JS
# Moved to sources        ${STMMI_GTK_HEADERS_DIR}/jsgtkdevicemanager.h
        )
# "SubProject" STMMI_GTK_HEADERS_ALL depends on
#   STMMI_GTK_HEADERS_MAS
#   STMMI_GTK_HEADERS_FLO
#   STMMI_GTK_HEADERS_JS
set(STMMI_GTK_HEADERS_ALL
        ${STMMI_GTK_HEADERS_DIR}/gtkdevicemanager.h
        ${STMMI_GTK_HEADERS_DIR}/stmm-input-gtk.h
        )
# Concatenate header file lists
#set(STMMI_GTK_HEADERS)
list(APPEND STMMI_GTK_HEADERS
        ${STMMI_GTK_HEADERS_DATA}
        ${STMMI_GTK_HEADERS_MAS}
        ${STMMI_GTK_HEADERS_FLO}
        ${STMMI_GTK_HEADERS_JS}
        ${STMMI_GTK_HEADERS_ALL}
        )
#
# Source files (and headers only used for building)
set(STMMI_GTK_SOURCES_DATA
        ${PROJECT_SOURCE_DIR}/src/gtkaccessor.cc
        ${PROJECT_SOURCE_DIR}/src/gdkkeyconverter.cc
        ${PROJECT_SOURCE_DIR}/src/gdkkeyconverterevdev.cc
        )
set(STMMI_GTK_SOURCES_MAS
        ${PROJECT_SOURCE_DIR}/src/masgtkdevicemanager.h
        ${PROJECT_SOURCE_DIR}/src/masgtkdevicemanager.cc
        ${PROJECT_SOURCE_DIR}/src/masgtkkeyboarddevice.h
        ${PROJECT_SOURCE_DIR}/src/masgtkkeyboarddevice.cc
        ${PROJECT_SOURCE_DIR}/src/masgtklistenerextradata.h
        ${PROJECT_SOURCE_DIR}/src/masgtkpointerdevice.h
        ${PROJECT_SOURCE_DIR}/src/masgtkpointerdevice.cc
        ${PROJECT_SOURCE_DIR}/src/masgtkwindowdata.h
        )
set(STMMI_GTK_SOURCES_FLO
        ${PROJECT_SOURCE_DIR}/src/floatingsources.h
        ${PROJECT_SOURCE_DIR}/src/floatingsources.cc
        ${PROJECT_SOURCE_DIR}/src/flogtkdevicemanager.h
        ${PROJECT_SOURCE_DIR}/src/flogtkdevicemanager.cc
        ${PROJECT_SOURCE_DIR}/src/flogtklistenerextradata.h
        ${PROJECT_SOURCE_DIR}/src/flogtkwindowdata.h
        ${PROJECT_SOURCE_DIR}/src/flogtkxkeyboarddevice.h
        ${PROJECT_SOURCE_DIR}/src/flogtkxkeyboarddevice.cc
        )
set(STMMI_GTK_SOURCES_JS
        ${PROJECT_SOURCE_DIR}/src/joysticksources.h
        ${PROJECT_SOURCE_DIR}/src/joysticksources.cc
        ${PROJECT_SOURCE_DIR}/src/jsgtkdevicemanager.h
        ${PROJECT_SOURCE_DIR}/src/jsgtkdevicemanager.cc
        ${PROJECT_SOURCE_DIR}/src/jsgtkjoystickdevice.h
        ${PROJECT_SOURCE_DIR}/src/jsgtkjoystickdevice.cc
        ${PROJECT_SOURCE_DIR}/src/jsgtklistenerextradata.h
        ${PROJECT_SOURCE_DIR}/src/jsgtkwindowdata.h
        )
set(STMMI_GTK_SOURCES_ALL
        ${PROJECT_SOURCE_DIR}/src/gtkdevicemanager.cc
        )
# Concatenate source file lists
list(APPEND  STMMI_GTK_SOURCES
        ${STMMI_GTK_SOURCES_DATA}
        ${STMMI_GTK_SOURCES_MAS}
        ${STMMI_GTK_SOURCES_FLO}
        ${STMMI_GTK_SOURCES_JS}
        ${STMMI_GTK_SOURCES_ALL}
        )

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

set(STMMI_COMPILE_WARNINGS "-Wall -Wextra -Werror") # "-Wpedantic"

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${STMMI_COMPILE_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STMMI_COMPILE_WARNINGS}")

# Set install dir for pkg-config file
set(STMMI_GTK_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
# Create pkg-config file for shared library
configure_file("${PROJECT_SOURCE_DIR}/stmm-input-gtk.pc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-gtk.pc" @ONLY)

# Define library
add_library(stmm-input-gtk SHARED ${STMMI_GTK_SOURCES})

message(STATUS "")
message(STATUS "stmm-input-gtk was configured with the following options:")
message(STATUS " CMAKE_BUILD_TYPE:             ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:        ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_FLAGS:              ${CMAKE_CXX_FLAGS}")
message(STATUS " STMMI_GTK_EXTRA_INCLUDE_DIRS: ${STMMI_GTK_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMI_GTK_EXTRA_LIBS:         ${STMMI_GTK_EXTRA_LIBS}")
message(STATUS " STMMI_GTK_HEADERS:            ${STMMI_GTK_HEADERS}")
message(STATUS " STMMI_GTK_SOURCES:            ${STMMI_GTK_SOURCES}")
message(STATUS " install prefix:               ${prefix}")
message(STATUS " BUILD_DOCS:                   ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                ${BUILD_TESTING}")

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-input-gtk doc)
endif()

# Testing
enable_testing()
add_subdirectory(test)

# Set library versions
set_target_properties(stmm-input-gtk PROPERTIES
        VERSION ${STMM_INPUT_GTK_VERSION}
        SOVERSION ${STMM_INPUT_GTK_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-gtk-${STMM_INPUT_GTK_VERSION}"
        )

# Determine header installation dir
target_include_directories(stmm-input-gtk PUBLIC
  $<BUILD_INTERFACE:${STMMI_GTK_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-gtk>  # <prefix>/include/stmm-input-gtk
)

target_link_libraries(stmm-input-gtk ${STMMI_GTK_EXTRA_LIBS}) # ${STMMI_GTK_EXTRA_LDFLAGS}

install(TARGETS stmm-input-gtk LIBRARY DESTINATION "lib")

install(FILES ${STMMI_GTK_HEADERS}   DESTINATION "include/stmm-input-gtk")

install(FILES  ${PROJECT_BINARY_DIR}/libstmm-input-gtk.so   DESTINATION "lib")

install(FILES  ${PROJECT_BINARY_DIR}/stmm-input-gtk.pc   DESTINATION "lib/pkgconfig")