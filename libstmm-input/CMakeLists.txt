if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the stmm-input source code and call cmake from there")
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


project(stmm-input CXX)

set(LIBRARY_OUTPUT_DIRECTORY "build")

#TODO remove: not used?
include(FindPkgConfig)
find_package(PkgConfig)

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_MAJOR_VERSION 0)
set(STMM_INPUT_MINOR_VERSION 1)
set(STMM_INPUT_VERSION "${STMM_INPUT_MAJOR_VERSION}.${STMM_INPUT_MINOR_VERSION}.0")

# Headers dir
set(STMMI_HEADERS_DIR  "${PROJECT_SOURCE_DIR}/include/stmm-input")
# Header files
set(STMMI_HEADERS
        ${STMMI_HEADERS_DIR}/accessor.h
        ${STMMI_HEADERS_DIR}/callifs.h
        ${STMMI_HEADERS_DIR}/capability.h
        ${STMMI_HEADERS_DIR}/device.h
        ${STMMI_HEADERS_DIR}/devicemanager.h
        ${STMMI_HEADERS_DIR}/event.h
        ${STMMI_HEADERS_DIR}/hardwarekey.h
        ${STMMI_HEADERS_DIR}/private-namedtypes.h
        ${STMMI_HEADERS_DIR}/stmm-input.h
        ${STMMI_HEADERS_DIR}/xyevent.h
        )
#
# Source files (and headers only used for building)
set(STMMI_SOURCES
        ${PROJECT_SOURCE_DIR}/src/accessor.cc
        ${PROJECT_SOURCE_DIR}/src/callifs.cc
        ${PROJECT_SOURCE_DIR}/src/capability.cc
        ${PROJECT_SOURCE_DIR}/src/device.cc
        ${PROJECT_SOURCE_DIR}/src/devicemanager.cc
        ${PROJECT_SOURCE_DIR}/src/event.cc
        ${PROJECT_SOURCE_DIR}/src/xyevent.cc
        )

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    set(STMMI_COMPILE_WARNINGS "-Wall -Wextra -Werror") # "-Wpedantic"
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(STMMI_COMPILE_WARNINGS "/W4 /WX /EHsc")
endif()
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} ${STMMI_COMPILE_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${STMMI_COMPILE_WARNINGS}")

# Set install dir for pkg-config file
set(STMMI_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
# Create pkg-config file for shared library
configure_file("${PROJECT_SOURCE_DIR}/stmm-input.pc.in"
               "${PROJECT_BINARY_DIR}/stmm-input.pc" @ONLY)

# Define library
add_library(stmm-input SHARED ${STMMI_SOURCES})

message(STATUS "")
message(STATUS "stmm-input was configured with the following options:")
message(STATUS " CMAKE_BUILD_TYPE:          ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:     ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_FLAGS:           ${CMAKE_CXX_FLAGS}")
message(STATUS " STMMI_HEADERS:             ${STMMI_HEADERS}")
message(STATUS " STMMI_SOURCES:             ${STMMI_SOURCES}")
message(STATUS " install prefix:            ${prefix}")
message(STATUS " BUILD_DOCS:                ${BUILD_DOCS}")

# Documentation
add_subdirectory(doc)
if (BUILD_DOCS)
    # build automatically, otherwise call `make doc`
    add_dependencies(stmm-input doc)
endif()

# Currently there are no tests:
# but to avoid cmake warning keep an empty condition
if (BUILD_TESTING)
endif()
#notests  # Testing
#notests  enable_testing()
#notests  add_subdirectory(test)


# Set library versions
set_target_properties(stmm-input PROPERTIES
        VERSION ${STMM_INPUT_VERSION}
        SOVERSION ${STMM_INPUT_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-${STMM_INPUT_VERSION}"
        )

# Determine header installation dir
target_include_directories(stmm-input PUBLIC
  $<BUILD_INTERFACE:${STMMI_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input>  # <prefix>/include/stmm-input
)

#target_link_libraries(stmm-input ${EXTRA_LIBS} ${EXTRA_LDFLAGS})

install(TARGETS stmm-input LIBRARY DESTINATION "lib")

install(FILES ${STMMI_HEADERS} DESTINATION "include/stmm-input")

install(FILES  ${PROJECT_BINARY_DIR}/libstmm-input.so   DESTINATION "lib")

install(FILES  ${PROJECT_BINARY_DIR}/stmm-input.pc   DESTINATION "lib/pkgconfig")

