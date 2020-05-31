if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the spinn source code and call cmake from there")
endif()

cmake_minimum_required(VERSION 3.0)


project(spinn CXX)

set(RUNTIME_OUTPUT_DIRECTORY "build")

include(FindPkgConfig)
find_package(PkgConfig)

# Version
set(SPINN_VERSION "0.13.0") # !-U-!

# Required libraries
pkg_check_modules(STMMINPUTGTKDM  REQUIRED  stmm-input-gtk-dm>=0.13) # !-U-!

list(APPEND SPINN_EXTRA_INCLUDE_DIRS   ${STMMINPUTGTKDM_INCLUDE_DIRS})
list(APPEND SPINN_EXTRA_LIBS           ${STMMINPUTGTKDM_LIBRARIES})

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

set(STMM_COMPILE_WARNINGS "-Wall -Wextra $ENV{STMM_CPP_OPTIONS}")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	string(REPLACE "-Wsuggest-override" "" STMM_COMPILE_WARNINGS ${STMM_COMPILE_WARNINGS})
	string(REPLACE "-Wlogical-op" "" STMM_COMPILE_WARNINGS ${STMM_COMPILE_WARNINGS})
endif()

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${STMM_COMPILE_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STMM_COMPILE_WARNINGS}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define executable
add_executable(spinn ${SPINN_SOURCES})

# Add include directories of used libraries
target_include_directories(spinn SYSTEM PUBLIC "${SPINN_EXTRA_INCLUDE_DIRS}")

target_link_libraries(spinn ${SPINN_EXTRA_LIBS})

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "spinn was configured with the following options:")
message(STATUS " SPINN_SOURCES:                  ${SPINN_SOURCES}")
message(STATUS " SPINN_DATA_FILES                ${SPINN_DATA_FILES}")
message(STATUS " SPINN_EXTRA_INCLUDE_DIRS:       ${SPINN_EXTRA_INCLUDE_DIRS}")
message(STATUS " SPINN_EXTRA_LIBS:               ${SPINN_EXTRA_LIBS}")
message(STATUS " CMAKE_BUILD_TYPE:               ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:          ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_FLAGS:                ${CMAKE_CXX_FLAGS}")
message(STATUS " CMAKE_CXX_FLAGS_DEBUG:          ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS " BUILD_TESTING                   ${BUILD_TESTING}")
message(STATUS " BUILD_SHARED_LIBS:              ${BUILD_SHARED_LIBS}")
endif()

# Testing
enable_testing()
add_subdirectory(test)

#install(TARGETS spinn RUNTIME DESTINATION "bin")
