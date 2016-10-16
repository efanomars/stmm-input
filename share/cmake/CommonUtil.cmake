# share/cmake/CommonUtil.cmake

function(CheckBinaryNotSourceTree)
    if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
        message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the ${CMAKE_PROJECT_NAME} source code and call cmake from there")
    endif()
endfunction(CheckBinaryNotSourceTree)


function(CheckBuildType)
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
endfunction(CheckBuildType)


function(DefineSharedLibOption)
    option(BUILD_SHARED_LIBS "Build shared library ${CMAKE_PROJECT_NAME}" ON)
    mark_as_advanced(BUILD_SHARED_LIBS)
endfunction(DefineSharedLibOption)


function(DefineCallerCompileOptions)
    if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
        set(STMMI_COMPILE_WARNINGS "-Wall -Wextra -Werror \
 -pedantic-errors -Wmissing-include-dirs -Winit-self \
 -Woverloaded-virtual") # "-Wpedantic -Wredundant-decls -Wsign-conversion"
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb"  PARENT_SCOPE)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(STMMI_COMPILE_WARNINGS "/W4 /WX /EHsc")
    endif()
    set(CMAKE_CXX_FLAGS       "${CMAKE_CXX_FLAGS} ${STMMI_COMPILE_WARNINGS} -std=c++14"  PARENT_SCOPE)
endfunction(DefineCallerCompileOptions)
