# File: libstmm-input-gtk-dm/CMakeLists.txt

cmake_minimum_required(VERSION 3.0)

option(OMIT_X11 "Exclude X11 (XI2) related device managers" OFF)
mark_as_advanced(OMIT_X11)

option(OMIT_PLUGINS "Exclude plugins device manager (stmm-input-dl)" OFF)
mark_as_advanced(OMIT_PLUGINS)

project(stmm-input-gtk-dm CXX)

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
set(STMM_INPUT_GTK_DM_MAJOR_VERSION 0)
set(STMM_INPUT_GTK_DM_MINOR_VERSION 1)
set(STMM_INPUT_GTK_DM_VERSION "${STMM_INPUT_GTK_DM_MAJOR_VERSION}.${STMM_INPUT_GTK_DM_MINOR_VERSION}.0")

# required stmm-input-gtk version
set(STMM_INPUT_GTK_MAJOR_VERSION_REQ 0)
set(STMM_INPUT_GTK_MINOR_VERSION_REQ 1)
set(STMM_INPUT_GTK_VERSION_REQ "${STMM_INPUT_GTK_MAJOR_VERSION_REQ}.${STMM_INPUT_GTK_MINOR_VERSION_REQ}")

# required stmm-input-ev version
set(STMM_INPUT_EV_MAJOR_VERSION_REQ 0)
set(STMM_INPUT_EV_MINOR_VERSION_REQ 1)
set(STMM_INPUT_EV_VERSION_REQ "${STMM_INPUT_EV_MAJOR_VERSION_REQ}.${STMM_INPUT_EV_MINOR_VERSION_REQ}")

if (NOT OMIT_PLUGINS)
    # required stmm-input-dl version
    set(STMM_INPUT_DL_MAJOR_VERSION_REQ 0)
    set(STMM_INPUT_DL_MINOR_VERSION_REQ 1)
    set(STMM_INPUT_DL_VERSION_REQ "${STMM_INPUT_DL_MAJOR_VERSION_REQ}.${STMM_INPUT_DL_MINOR_VERSION_REQ}")
endif()

set(GTKMM_VERSION_REQ "3.14.0")
set(XI_VERSION_REQ "1.7.4")
set(X11_VERSION_REQ "1.6.2")

# Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
pkg_check_modules(STMMINPUTGTK    REQUIRED  stmm-input-gtk>=${STMM_INPUT_GTK_VERSION_REQ})
pkg_check_modules(STMMINPUTEV    REQUIRED  stmm-input-ev>=${STMM_INPUT_EV_VERSION_REQ})
set(STMMI_GTK_DM_DL_DEPENDENCIES )
if (NOT OMIT_PLUGINS)
    pkg_check_modules(STMMINPUTDL    REQUIRED  stmm-input-dl>=${STMM_INPUT_DL_VERSION_REQ})
    # Beware! The space between the library name and the comparison is necessary!
    set(STMMI_GTK_DM_DL_DEPENDENCIES "stmm-input-dl >= ${STMM_INPUT_DL_VERSION_REQ}")
endif()
pkg_check_modules(GTKMM  REQUIRED  gtkmm-3.0>=${GTKMM_VERSION_REQ})
set(STMMI_GTK_DM_X11_DEPENDENCIES )
if (NOT OMIT_X11)
    pkg_check_modules(XI     REQUIRED  xi>=${XI_VERSION_REQ})
    pkg_check_modules(X11    REQUIRED  x11>=${X11_VERSION_REQ})
    # Beware! The space between the library name and the comparison is necessary!
    set(STMMI_GTK_DM_X11_DEPENDENCIES "xi >= ${XI_VERSION_REQ}  x11 >= ${X11_VERSION_REQ}")
endif()

list(APPEND STMMI_GTK_DM_EXTRA_INCLUDE_DIRS
    ${STMMINPUTGTK_INCLUDE_DIRS}
    ${STMMINPUTEV_INCLUDE_DIRS}
    ${GTKMM_INCLUDE_DIRS}
    )
if (NOT OMIT_X11)
    list(APPEND STMMI_GTK_DM_EXTRA_INCLUDE_DIRS
        ${XI_INCLUDE_DIRS}
        ${X11_INCLUDE_DIRS}
        )
endif()
if (NOT OMIT_PLUGINS)
    list(APPEND STMMI_GTK_DM_EXTRA_INCLUDE_DIRS
        ${STMMINPUTDL_INCLUDE_DIRS}
        )
endif()
list(APPEND STMMI_GTK_DM_EXTRA_LIBS
    ${STMMINPUTGTK_LIBRARIES}
    ${STMMINPUTEV_LIBRARIES}
    ${GTKMM_LIBRARIES}
    )
if (NOT OMIT_X11)
    list(APPEND STMMI_GTK_DM_EXTRA_LIBS
        ${XI_LIBRARIES}
        ${X11_LIBRARIES}
        )
endif()
if (NOT OMIT_PLUGINS)
    list(APPEND STMMI_GTK_DM_EXTRA_LIBS
        ${STMMINPUTDL_LIBRARIES}
        )
endif()

string(STRIP "${STMMI_GTK_DM_EXTRA_LIBS}"         STMMI_GTK_DM_EXTRA_LIBS)
string(STRIP "${STMMI_GTK_DM_EXTRA_INCLUDE_DIRS}" STMMI_GTK_DM_EXTRA_INCLUDE_DIRS)

include_directories("${STMMI_GTK_DM_EXTRA_INCLUDE_DIRS}")

# Headers dir
set(STMMI_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/include")
set(STMMI_HEADERS_DIR  "${STMMI_INCLUDE_DIR}/stmm-input-gtk-dm")
# Header files
# "SubProject" STMMI_HEADERS_DATA depends on
#   libstmm-input
#   gtkmm-3.0-dev
set(STMMI_HEADERS_DATA
        ${STMMI_HEADERS_DIR}/gdkkeyconverter.h
        ${STMMI_HEADERS_DIR}/gdkkeyconverterevdev.h
        ${STMMI_HEADERS_DIR}/jsdevicefiles.h
        ${STMMI_HEADERS_DIR}/keyrepeatmode.h
        )
# "SubProject" STMMI_HEADERS_MAS depends on
#   STMMI_HEADERS_DATA
#   libstmm-input-ev
set(STMMI_HEADERS_MAS
# Moved to sources        ${STMMI_HEADERS_DIR}/masgtkdevicemanager.h
        )
# "SubProject" STMMI_HEADERS_FLO depends on
#   STMMI_HEADERS_DATA
#   libstmm-input-ev
#   libxi-dev
#   libx11-dev
if (NOT OMIT_X11)
    set(STMMI_HEADERS_FLO
    # Moved to sources        ${STMMI_HEADERS_DIR}/flogtkdevicemanager.h
            )
else()
    set(STMMI_HEADERS_FLO)
endif()
# "SubProject" STMMI_HEADERS_JS depends on
#   STMMI_HEADERS_DATA
#   libstmm-input-ev
set(STMMI_HEADERS_JS
# Moved to sources        ${STMMI_HEADERS_DIR}/jsgtkdevicemanager.h
        )
# "SubProject" STMMI_HEADERS_ALL depends on
#   STMMI_HEADERS_MAS
#   STMMI_HEADERS_FLO
#   STMMI_HEADERS_JS
#   libstmm-input-dl
set(STMMI_HEADERS_ALL
        ${STMMI_HEADERS_DIR}/gtkdevicemanager.h
        ${STMMI_HEADERS_DIR}/stmm-input-gtk-dm.h
        ${STMMI_HEADERS_DIR}/stmm-input-gtk-dm-config.h
        )
# Concatenate header file lists
#set(STMMI_HEADERS)
list(APPEND STMMI_HEADERS
        ${STMMI_HEADERS_DATA}
        ${STMMI_HEADERS_MAS}
        ${STMMI_HEADERS_FLO}
        ${STMMI_HEADERS_JS}
        ${STMMI_HEADERS_ALL}
        )
#
# Sources dir
set(STMMI_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/src")
# Source files (and headers only used for building)
set(STMMI_SOURCES_DATA
        ${STMMI_SOURCES_DIR}/gdkkeyconverterevdev.cc
        ${STMMI_SOURCES_DIR}/recycler.h
        )
set(STMMI_SOURCES_MAS
        ${STMMI_SOURCES_DIR}/masgtkbackend.h
        ${STMMI_SOURCES_DIR}/masgtkbackend.cc
        ${STMMI_SOURCES_DIR}/masgtkdevicemanager.h
        ${STMMI_SOURCES_DIR}/masgtkdevicemanager.cc
        ${STMMI_SOURCES_DIR}/masgtkkeyboarddevice.h
        ${STMMI_SOURCES_DIR}/masgtkkeyboarddevice.cc
        ${STMMI_SOURCES_DIR}/masgtklistenerextradata.h
        ${STMMI_SOURCES_DIR}/masgtkpointerdevice.h
        ${STMMI_SOURCES_DIR}/masgtkpointerdevice.cc
        ${STMMI_SOURCES_DIR}/masgtkwindowdata.h
        ${STMMI_SOURCES_DIR}/masgtkwindowdata.cc
        )
if (NOT OMIT_X11)
    set(STMMI_SOURCES_FLO
            ${STMMI_SOURCES_DIR}/flogtkbackend.h
            ${STMMI_SOURCES_DIR}/flogtkbackend.cc
            ${STMMI_SOURCES_DIR}/floatingsources.h
            ${STMMI_SOURCES_DIR}/floatingsources.cc
            ${STMMI_SOURCES_DIR}/flogtkdevicemanager.h
            ${STMMI_SOURCES_DIR}/flogtkdevicemanager.cc
            ${STMMI_SOURCES_DIR}/flogtklistenerextradata.h
            ${STMMI_SOURCES_DIR}/flogtkwindowdata.h
            ${STMMI_SOURCES_DIR}/flogtkwindowdata.cc
            ${STMMI_SOURCES_DIR}/flogtkxkeyboarddevice.h
            ${STMMI_SOURCES_DIR}/flogtkxkeyboarddevice.cc
            )
else()
    set(STMMI_SOURCES_FLO)
endif()
set(STMMI_SOURCES_JS
        ${STMMI_SOURCES_DIR}/jsgtkbackend.h
        ${STMMI_SOURCES_DIR}/jsgtkbackend.cc
        ${STMMI_SOURCES_DIR}/joysticksources.h
        ${STMMI_SOURCES_DIR}/joysticksources.cc
        ${STMMI_SOURCES_DIR}/jsgtkdevicemanager.h
        ${STMMI_SOURCES_DIR}/jsgtkdevicemanager.cc
        ${STMMI_SOURCES_DIR}/jsgtkjoystickdevice.h
        ${STMMI_SOURCES_DIR}/jsgtkjoystickdevice.cc
        ${STMMI_SOURCES_DIR}/jsgtklistenerextradata.h
        ${STMMI_SOURCES_DIR}/jsgtkwindowdata.h
        )
set(STMMI_SOURCES_ALL
        ${STMMI_SOURCES_DIR}/gtkdevicemanager.cc
        )
# Concatenate source file lists
list(APPEND  STMMI_SOURCES
        ${STMMI_SOURCES_DATA}
        ${STMMI_SOURCES_MAS}
        ${STMMI_SOURCES_FLO}
        ${STMMI_SOURCES_JS}
        ${STMMI_SOURCES_ALL}
        )

# Define library
add_library(stmm-input-gtk-dm ${STMMI_SOURCES} ${PROJECT_BINARY_DIR}/stmm-input-gtk-dm-config.cc)

DefineTargetPublicCompileOptions(stmm-input-gtk-dm)

if (OMIT_X11)
    target_compile_definitions(stmm-input-gtk-dm PUBLIC STMI_OMIT_X11)
endif()
if (OMIT_PLUGINS)
    target_compile_definitions(stmm-input-gtk-dm PUBLIC STMI_OMIT_PLUGINS)
endif()

# Set version for stmm-input-gtk-dm-config.cc.in
set(STMMI_PKG_VERSION "${STMM_INPUT_GTK_DM_VERSION}")
# Create config file for library
configure_file("${PROJECT_SOURCE_DIR}/src/stmm-input-gtk-dm-config.cc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-gtk-dm-config.cc" @ONLY)

# Set install dir for pkg-config file
set(STMMI_GTK_DM_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
# Create pkg-config file for shared library
configure_file("${PROJECT_SOURCE_DIR}/stmm-input-gtk-dm.pc.in"
               "${PROJECT_BINARY_DIR}/stmm-input-gtk-dm.pc" @ONLY)

message(STATUS "")
message(STATUS "stmm-input-gtk-dm was configured with the following options:")
message(STATUS " STMMI_HEADERS:                   ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:                   ${STMMI_SOURCES}")
message(STATUS " STMMI_GTK_DM_EXTRA_INCLUDE_DIRS: ${STMMI_GTK_DM_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMI_GTK_DM_EXTRA_LIBS:         ${STMMI_GTK_DM_EXTRA_LIBS}")
message(STATUS " CMAKE_BUILD_TYPE:                ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:           ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:      ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                 ${CMAKE_CXX_FLAGS}")
message(STATUS " install prefix:                  ${STMMI_GTK_DM_INSTALL_PREFIX}")
message(STATUS " BUILD_DOCS:                      ${BUILD_DOCS}")
message(STATUS " BUILD_TESTING:                   ${BUILD_TESTING}")
message(STATUS " OMIT_X11:                        ${OMIT_X11}")
message(STATUS " OMIT_PLUGINS:                    ${OMIT_PLUGINS}")

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-input-gtk-dm doc)
endif()

# Testing
add_subdirectory(googletest)
enable_testing()
add_subdirectory(test)

# Set library versions
set_target_properties(stmm-input-gtk-dm PROPERTIES
        VERSION ${STMM_INPUT_GTK_DM_VERSION}
        SOVERSION ${STMM_INPUT_GTK_DM_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-gtk-dm-${STMM_INPUT_GTK_DM_VERSION}"
        )

# Determine header installation dir
target_include_directories(stmm-input-gtk-dm    BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-gtk-dm>  # <prefix>/include/stmm-input-gtk-dm
)

target_link_libraries(stmm-input-gtk-dm ${STMMI_GTK_DM_EXTRA_LIBS}) # ${STMMI_GTK_DM_EXTRA_LDFLAGS}

install(TARGETS stmm-input-gtk-dm LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")

install(FILES ${STMMI_HEADERS}   DESTINATION "include/stmm-input-gtk-dm")

install(FILES  ${PROJECT_BINARY_DIR}/stmm-input-gtk-dm.pc   DESTINATION "lib/pkgconfig")