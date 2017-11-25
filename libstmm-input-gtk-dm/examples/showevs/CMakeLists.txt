if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the showevs source code and call cmake from there")
endif()

cmake_minimum_required(VERSION 3.0)

project(showevs CXX)

set(RUNTIME_OUTPUT_DIRECTORY "build")

include(FindPkgConfig)
find_package(PkgConfig)

# Version
set(SHOWEVS_VERSION "0.1.0")

# Required libraries
pkg_check_modules(STMMINPUTGTKDM  REQUIRED  stmm-input-gtk-dm>=0.1.0)

list(APPEND SHOWEVS_EXTRA_INCLUDE_DIRS   ${STMMINPUTGTKDM_INCLUDE_DIRS})
list(APPEND SHOWEVS_EXTRA_LIBS           ${STMMINPUTGTKDM_LIBRARIES})

# Add include directories of used libraries
include_directories(SYSTEM "${SHOWEVS_EXTRA_INCLUDE_DIRS}")

# Source and headers files
set(SHOWEVS_SOURCES
        ${PROJECT_SOURCE_DIR}/src/inputstrings.h
        ${PROJECT_SOURCE_DIR}/src/inputstrings.cc
        ${PROJECT_SOURCE_DIR}/src/main.cc
        ${PROJECT_SOURCE_DIR}/src/showevswindow.h
        ${PROJECT_SOURCE_DIR}/src/showevswindow.cc
        )
# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

set(STMM_COMPILE_WARNINGS "-Wall -Wextra $ENV{STMM_CPP_OPTIONS}")

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${STMM_COMPILE_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STMM_COMPILE_WARNINGS}")

# Define executable
add_executable(showevs ${SHOWEVS_SOURCES})

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "showevs was configured with the following options:")
message(STATUS " SHOWEVS_SOURCES:                ${SHOWEVS_SOURCES}")
message(STATUS " SHOWEVS_DATA_FILES              ${SHOWEVS_DATA_FILES}")
message(STATUS " SHOWEVS_EXTRA_INCLUDE_DIRS:     ${SHOWEVS_EXTRA_INCLUDE_DIRS}")
message(STATUS " SHOWEVS_EXTRA_LIBS:             ${SHOWEVS_EXTRA_LIBS}")
message(STATUS " CMAKE_BUILD_TYPE:               ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:          ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_FLAGS:                ${CMAKE_CXX_FLAGS}")
message(STATUS " CMAKE_CXX_FLAGS_DEBUG:          ${CMAKE_CXX_FLAGS_DEBUG}")
#message(STATUS " install prefix:                 ${CMAKE_INSTALL_PREFIX}")
endif()

target_link_libraries(showevs ${SHOWEVS_EXTRA_LIBS})

#install(TARGETS showevs RUNTIME DESTINATION "bin")
