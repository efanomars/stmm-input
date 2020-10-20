if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the bare-app source code and call cmake from there")
endif()

cmake_minimum_required(VERSION 3.0)

project(bare-app CXX)

set(RUNTIME_OUTPUT_DIRECTORY "build")

include(FindPkgConfig)
if (NOT PKG_CONFIG_FOUND)
     message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
endif()

# Version
set(BARE_APP_VERSION "0.15") # !-U-!

# Required libraries
set(BARE_APP_REQ_STMM_INPUT_GTK_DM_VERSION "0.15") # !-U-!

# Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
pkg_check_modules(STMMINPUTGTKDM  REQUIRED  stmm-input-gtk-dm>=${BARE_APP_REQ_STMM_INPUT_GTK_DM_VERSION})

list(APPEND BARE_APP_EXTRA_INCLUDE_DIRS   ${STMMINPUTGTKDM_INCLUDE_DIRS})
list(APPEND BARE_APP_EXTRA_LIBS           ${STMMINPUTGTKDM_LIBRARIES})

# Add include directories of used libraries
include_directories(SYSTEM "${BARE_APP_EXTRA_INCLUDE_DIRS}")

# Source and headers files
set(BARE_APP_SOURCES
        ${PROJECT_SOURCE_DIR}/src/barewindow.h
        ${PROJECT_SOURCE_DIR}/src/barewindow.cc
        ${PROJECT_SOURCE_DIR}/src/main.cc
        )
# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

set(STMM_COMPILE_WARNINGS "-Wall -Wextra $ENV{STMM_CPP_OPTIONS}")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	string(REPLACE "-Wsuggest-override" "" STMM_COMPILE_WARNINGS ${STMM_COMPILE_WARNINGS})
	string(REPLACE "-Wlogical-op" "" STMM_COMPILE_WARNINGS ${STMM_COMPILE_WARNINGS})
endif()

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${STMM_COMPILE_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STMM_COMPILE_WARNINGS}")

# Define executable
add_executable(bare-app ${BARE_APP_SOURCES})

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "bare-app was configured with the following options:")
message(STATUS " BARE_APP_SOURCES:                ${BARE_APP_SOURCES}")
message(STATUS " BARE_APP_DATA_FILES              ${BARE_APP_DATA_FILES}")
message(STATUS " BARE_APP_EXTRA_INCLUDE_DIRS:     ${BARE_APP_EXTRA_INCLUDE_DIRS}")
message(STATUS " BARE_APP_EXTRA_LIBS:             ${BARE_APP_EXTRA_LIBS}")
message(STATUS " CMAKE_BUILD_TYPE:                ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:           ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_FLAGS:                 ${CMAKE_CXX_FLAGS}")
message(STATUS " CMAKE_CXX_FLAGS_DEBUG:           ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS " install prefix:                  ${CMAKE_INSTALL_PREFIX}")
endif()

target_link_libraries(bare-app ${BARE_APP_EXTRA_LIBS})

#install(TARGETS bare-app RUNTIME DESTINATION "bin")
