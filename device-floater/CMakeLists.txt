if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the device-floater source code and call cmake from there")
endif()

option(bMakeRawDebianPackage "Define raw debian binary package. Only if not Debug. Use `make package` to create it."  OFF)

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

project(device-floater CXX)

set(RUNTIME_OUTPUT_DIRECTORY "build")

include(FindPkgConfig)
find_package(PkgConfig)

#
set(DEVICE_FLOATER_MAJOR_VERSION 0)
set(DEVICE_FLOATER_MINOR_VERSION 1)
set(DEVICE_FLOATER_VERSION "${DEVICE_FLOATER_MAJOR_VERSION}.${DEVICE_FLOATER_MINOR_VERSION}.0")

set(GTKMM_VERSION_REQ "3.4.2")
set(XI_VERSION_REQ "1.7.4")
set(X11_VERSION_REQ "1.6.2")

pkg_check_modules(GTKMM  REQUIRED  gtkmm-3.0>=${GTKMM_VERSION_REQ})
pkg_check_modules(XI  REQUIRED  xi>=${XI_VERSION_REQ})
pkg_check_modules(X11  REQUIRED  x11>=${X11_VERSION_REQ})

list(APPEND STMMI_DEVFLO_EXTRA_INCLUDE_DIRS   ${GTKMM_INCLUDE_DIRS} ${XI_INCLUDE_DIRS} ${X11_INCLUDE_DIRS})
list(APPEND STMMI_DEVFLO_EXTRA_LIBS           ${GTKMM_LIBRARIES}    ${XI_LIBRARIES}    ${X11_LIBRARIES})

string(STRIP "${STMMI_DEVFLO_EXTRA_LIBS}" STMMI_DEVFLO_EXTRA_LIBS)
string(STRIP "${STMMI_DEVFLO_EXTRA_INCLUDE_DIRS}" STMMI_DEVFLO_EXTRA_INCLUDE_DIRS)

include_directories("${STMMI_DEVFLO_EXTRA_INCLUDE_DIRS}")

# Source files (and headers only used for building)
set(STMMI_DEVFLO_SOURCES
        ${PROJECT_SOURCE_DIR}/src/devicefloater.h
        ${PROJECT_SOURCE_DIR}/src/devicefloater.cc
        ${PROJECT_SOURCE_DIR}/src/main.cc
        ${PROJECT_SOURCE_DIR}/src/xideviceswindow.h
        ${PROJECT_SOURCE_DIR}/src/xideviceswindow.cc
        )
set(STMMI_DEVFLO_DATA_FILES
        ${PROJECT_SOURCE_DIR}/data/buoy_32313.png
        ${PROJECT_SOURCE_DIR}/data/keyboard_1420.png
        ${PROJECT_SOURCE_DIR}/data/mouse_347.png
        )
# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

set(STMM_COMPILE_WARNINGS "-Wall -Wextra -Werror") # "-Wpedantic"

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${STMM_COMPILE_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STMM_COMPILE_WARNINGS}")

include(GNUInstallDirs)
set(STMMI_DEVFLO_PKG_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/device-floater")
set(STMMI_DEVFLO_PKG_REL_DATA_DIR "${CMAKE_INSTALL_DATADIR}/device-floater")

# Create config file for executable
configure_file("${PROJECT_SOURCE_DIR}/src/config.cc.in"
               "${PROJECT_BINARY_DIR}/config.cc" @ONLY)
# This allows config.cc to find the config.h include
include_directories("${PROJECT_SOURCE_DIR}/src")

add_executable(device-floater ${STMMI_DEVFLO_SOURCES} ${PROJECT_BINARY_DIR}/config.cc)

message(STATUS "")
message(STATUS "device-floater was configured with the following options:")
message(STATUS " CMAKE_BUILD_TYPE:                ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:           ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_FLAGS:                 ${CMAKE_CXX_FLAGS}")
message(STATUS " CMAKE_CXX_FLAGS_DEBUG:           ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS " STMMI_DEVFLO_EXTRA_INCLUDE_DIRS: ${STMMI_DEVFLO_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMI_DEVFLO_EXTRA_LIBS:         ${STMMI_DEVFLO_EXTRA_LIBS}")
message(STATUS " STMMI_DEVFLO_SOURCES:            ${STMMI_DEVFLO_SOURCES}")
message(STATUS " STMMI_DEVFLO_DATA_FILES          ${STMMI_DEVFLO_DATA_FILES}")
message(STATUS " install prefix:                  ${CMAKE_INSTALL_PREFIX}")

#notestsyet  # Testing
#notestsyet  enable_testing()
#notestsyet  add_subdirectory(test)

target_link_libraries(device-floater ${STMMI_DEVFLO_EXTRA_LIBS}) # ${STMMI_DEVFLO_EXTRA_LDFLAGS}

install(TARGETS device-floater RUNTIME DESTINATION "bin")

install(FILES ${STMMI_DEVFLO_DATA_FILES}  DESTINATION   ${STMMI_DEVFLO_PKG_REL_DATA_DIR})

