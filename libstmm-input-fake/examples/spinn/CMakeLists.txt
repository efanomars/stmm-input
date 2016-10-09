if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the spinn source code and call cmake from there")
endif()

cmake_minimum_required(VERSION 3.0)


project(spinn CXX)

set(RUNTIME_OUTPUT_DIRECTORY "build")

include(FindPkgConfig)
find_package(PkgConfig)

# Version
set(SPINN_VERSION "0.1.0")

# Required libraries
pkg_check_modules(STMM_INPUT_GTK  REQUIRED  stmm-input-gtk>=0.1.0)

list(APPEND SPINN_EXTRA_INCLUDE_DIRS   ${STMM_INPUT_GTK_INCLUDE_DIRS})
list(APPEND SPINN_EXTRA_LIBS           ${STMM_INPUT_GTK_LIBRARIES})

# Add include directories of used libraries
include_directories("${SPINN_EXTRA_INCLUDE_DIRS}")

# Source and headers files
set(SPINN_SOURCES
        ${PROJECT_SOURCE_DIR}/src/spinn.h
        ${PROJECT_SOURCE_DIR}/src/spinn.cc
        ${PROJECT_SOURCE_DIR}/src/main.cc
        ${PROJECT_SOURCE_DIR}/src/spinnwindow.h
        ${PROJECT_SOURCE_DIR}/src/spinnwindow.cc
        )
# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

set(STMM_COMPILE_WARNINGS "-Wall -Wextra -Werror") # "-Wpedantic"

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${STMM_COMPILE_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STMM_COMPILE_WARNINGS}")

# Define executable
add_executable(spinn ${SPINN_SOURCES})

message(STATUS "")
message(STATUS "spinn was configured with the following options:")
message(STATUS " CMAKE_BUILD_TYPE:               ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:          ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_FLAGS:                ${CMAKE_CXX_FLAGS}")
message(STATUS " CMAKE_CXX_FLAGS_DEBUG:          ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS " SPINN_EXTRA_INCLUDE_DIRS:       ${SPINN_EXTRA_INCLUDE_DIRS}")
message(STATUS " SPINN_EXTRA_LIBS:               ${SPINN_EXTRA_LIBS}")
message(STATUS " SPINN_SOURCES:                  ${SPINN_SOURCES}")
message(STATUS " SPINN_DATA_FILES                ${SPINN_DATA_FILES}")
#message(STATUS " install prefix:                 ${CMAKE_INSTALL_PREFIX}")
message(STATUS " BUILD_TESTING                   ${BUILD_TESTING}")

# Testing
enable_testing()
add_subdirectory(test)

target_link_libraries(spinn ${SPINN_EXTRA_LIBS})

#install(TARGETS spinn RUNTIME DESTINATION "bin")