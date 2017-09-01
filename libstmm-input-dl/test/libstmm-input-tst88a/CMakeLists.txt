# File: libstmm-input-tst88a/CMakeLists.txt

# cmake_minimum_required(VERSION 3.0)
# 
# project(stmm-input-tst88a CXX)
# 
# set(LIBRARY_OUTPUT_DIRECTORY "build")
# 
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../share/cmake) 
# 
# include(CommonUtil)
# 
# CheckBinaryNotSourceTree()
# CheckBuildType()
# DefineSharedLibOption()
# DefineCommonOptions()
# DefineCommonCompileOptions()

# Libtool CURRENT/REVISION/AGE: here
#   MAJOR is CURRENT interface
#   MINOR is REVISION (implementation of interface)
#   AGE is always 0
set(STMM_INPUT_TST88A_MAJOR_VERSION 0)
set(STMM_INPUT_TST88A_MINOR_VERSION 1)
set(STMM_INPUT_TST88A_VERSION "${STMM_INPUT_TST88A_MAJOR_VERSION}.${STMM_INPUT_TST88A_MINOR_VERSION}.0")
# required stmm-input version
set(STMM_INPUT_MAJOR_VERSION_REQ 0)
set(STMM_INPUT_MINOR_VERSION_REQ 1)
set(STMM_INPUT_VERSION_REQ "${STMM_INPUT_MAJOR_VERSION_REQ}.${STMM_INPUT_MINOR_VERSION_REQ}")

include(FindPkgConfig)

if (NOT PKG_CONFIG_FOUND)
     message(FATAL_ERROR "Mandatory 'pkg-config' not found!")
endif()
# if (PKG_CONFIG_FOUND)
#     # Beware! The prefix passed to pkg_check_modules(PREFIX ...) shouldn't contain underscores!
#     pkg_check_modules(STMMINPUT  REQUIRED  stmm-input>=${STMM_INPUT_VERSION_REQ})
# else (PKG_CONFIG_FOUND)
#     list(APPEND STMMINPUT_INCLUDE_DIRS
#             "${PROJECT_SOURCE_DIR}/../libstmm-input/include")
#     list(APPEND STMMINPUT_LIBRARIES
#             "${PROJECT_SOURCE_DIR}/../libstmm-input/build/libstmm-input.a")
# endif()

pkg_check_modules(STMMINPUT  REQUIRED  stmm-input>=${STMM_INPUT_VERSION_REQ})

list(APPEND STMMI_TST88A_EXTRA_INCLUDE_DIRS  ${STMMINPUT_INCLUDE_DIRS})
list(APPEND STMMI_TST88A_EXTRA_LIBS          ${STMMINPUT_LIBRARIES})

string(STRIP "${STMMI_TST88A_EXTRA_LIBS}"         STMMI_TST88A_EXTRA_LIBS)
string(STRIP "${STMMI_TST88A_EXTRA_INCLUDE_DIRS}" STMMI_TST88A_EXTRA_INCLUDE_DIRS)


# Headers dir
set(STMMI_TST88A_INCLUDE_DIR  "${PROJECT_SOURCE_DIR}/test/libstmm-input-tst88a/include")
set(STMMI_TST88A_HEADERS_DIR  "${STMMI_TST88A_INCLUDE_DIR}/stmm-input-tst88a")
# Header files
set(STMMI_TST88A_HEADERS
        ${STMMI_TST88A_HEADERS_DIR}/tst88capability.h
      )
#
# Sources dir
set(STMMI_TST88A_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test/libstmm-input-tst88a/src")
# Source files (and headers only used for building)
set(STMMI_TST88A_SOURCES
        ${STMMI_TST88A_SOURCES_DIR}/tst88capability.cc
      )

# Define library
add_library(stmm-input-tst88a ${STMMI_TST88A_SOURCES}) # SHARED

target_include_directories(stmm-input-tst88a PUBLIC "${STMMI_TST88A_EXTRA_INCLUDE_DIRS}")

DefineTargetPublicCompileOptions(stmm-input-tst88a)

# # Set version for stmm-input-tst88a-config.cc.in
# set(STMMI_PKG_VERSION "${STMM_INPUT_TST88A_VERSION}")
# # Create config file for library
# configure_file("${PROJECT_SOURCE_DIR}/src/stmm-input-tst88a-config.cc.in"
#                "${PROJECT_BINARY_DIR}/stmm-input-tst88a-config.cc" @ONLY)

## Set install dir for pkg-config file
#set(STMMI_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
#
## Create pkg-config file for shared library
#configure_file("${PROJECT_SOURCE_DIR}/test/libstmm-input-tst88a/stmm-input-tst88a.pc.in"
#               "${PROJECT_BINARY_DIR}/stmm-input-tst88a.pc" @ONLY)

if ($ENV{STMM_CMAKE_COMMENTS})
message(STATUS "")
message(STATUS "stmm-input-tst88a was configured with the following options:")
message(STATUS " STMMI_TST88A_HEADERS:            ${STMMI_TST88A_HEADERS}")
message(STATUS " STMMI_TST88A_SOURCES:            ${STMMI_TST88A_SOURCES}")
message(STATUS " STMMI_TST88A_EXTRA_INCLUDE_DIRS: ${STMMI_TST88A_EXTRA_INCLUDE_DIRS}")
message(STATUS " STMMI_TST88A_EXTRA_LIBS:         ${STMMI_TST88A_EXTRA_LIBS}")
message(STATUS " CMAKE_BUILD_TYPE:                ${CMAKE_BUILD_TYPE}")
message(STATUS " CMAKE_CXX_LINK_EXECUTABLE        ${CMAKE_CXX_LINK_EXECUTABLE}")
message(STATUS " CMAKE_CXX_COMPILER_ID:           ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " CMAKE_CXX_COMPILER_VERSION:      ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " CMAKE_CXX_FLAGS:                 ${CMAKE_CXX_FLAGS}")
endif()

# # Documentation
# add_subdirectory(doc)
# if (BUILD_DOCS)
#     # build automatically, otherwise call `make doc`
#     add_dependencies(stmm-input-tst88a doc)
# endif()
# 
# # Testing
# add_subdirectory(googletest)
# enable_testing()
# add_subdirectory(test)

# Set library versions
set_target_properties(stmm-input-tst88a   PROPERTIES
        VERSION ${STMM_INPUT_TST88A_VERSION}
        SOVERSION ${STMM_INPUT_TST88A_MAJOR_VERSION}
#        OUTPUT_NAME "stmm-input-tst88a-${STMM_INPUT_TST88A_VERSION}"
        )

# Determine header installation dir
target_include_directories(stmm-input-tst88a   BEFORE PUBLIC
  $<BUILD_INTERFACE:${STMMI_TST88A_HEADERS_DIR}>
  $<INSTALL_INTERFACE:include/stmm-input-tst88a>  # <prefix>/include/stmm-input-tst88a
)

target_link_libraries(stmm-input-tst88a ${STMMI_TST88A_EXTRA_LIBS}) # ${STMMI_TST88A_EXTRA_LDFLAGS}

#install(TARGETS stmm-input-tst88a LIBRARY DESTINATION "lib"  ARCHIVE DESTINATION "lib")
#
#install(FILES ${STMMI_TST88A_HEADERS} DESTINATION "include/stmm-input-tst88a")
#
#install(FILES  ${PROJECT_BINARY_DIR}/stmm-input-tst88a.pc   DESTINATION "lib/pkgconfig")
