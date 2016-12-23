# share/cmake/CommonUtil.cmake

function(CheckBinaryNotSourceTree)
    if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
        message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the ${CMAKE_PROJECT_NAME} source code and call cmake from there")
    endif()
endfunction(CheckBinaryNotSourceTree)


function(CheckBuildType)
    string(STRIP "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE)

    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(CMAKE_EXPORT_COMPILE_COMMANDS ON    PARENT_SCOPE)
    elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        #set(STMM_RELEASE)
    elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel")
        #set(STMM_MINSIZEREL)
    elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
        #set(STMM_RELWITHDEBINFO)
    else()
        message(FATAL_ERROR "Wrong CMAKE_BUILD_TYPE, set to either Debug, Release, MinSizeRel or RelWithDebInfo")
    endif()
endfunction(CheckBuildType)


function(DefineSharedLibOption)
    option(BUILD_SHARED_LIBS "Build shared library ${CMAKE_PROJECT_NAME}" ON)
    mark_as_advanced(BUILD_SHARED_LIBS)
endfunction(DefineSharedLibOption)

function(DefineCommonOptions)
    option(BUILD_WITH_SANITIZE "Build with sanitize=address (Debug only!)" OFF)
    mark_as_advanced(BUILD_WITH_SANITIZE)
endfunction(DefineCommonOptions)


function(DefineCommonCompileOptions)
    # Compiler option also used for googletest
    string(STRIP "${CMAKE_CXX_FLAGS} -std=c++14" CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" PARENT_SCOPE)
    if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
        if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            if (BUILD_WITH_SANITIZE)
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address" PARENT_SCOPE)
            endif()
        endif()
    endif()
endfunction(DefineCommonCompileOptions)

function(DefineTargetInterfaceCompileOptions STMMI_TARGET)
    DefineTargetCompileOptionsType(${STMMI_TARGET} ON)
endfunction(DefineTargetInterfaceCompileOptions)

function(DefineTargetPublicCompileOptions STMMI_TARGET)
    DefineTargetCompileOptionsType(${STMMI_TARGET} OFF)
endfunction(DefineTargetPublicCompileOptions)

function(DefineTargetCompileOptionsType STMMI_TARGET STMMI_INTERFACE_TYPE)
    # STMMI_COMPILE_WARNINGS holds target specific flags
    set(STMMI_COMPILE_WARNINGS "${CMAKE_CXX_FLAGS}")
    if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
        set(STMMI_COMPILE_WARNINGS "${STMMI_COMPILE_WARNINGS} -Wall -Wextra -Werror \
-pedantic-errors -Wmissing-include-dirs -Winit-self \
-Woverloaded-virtual") # "-Wpedantic -Wredundant-decls -Wsign-conversion"
        if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") AND NOT (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.6))
            set(STMMI_COMPILE_WARNINGS "${STMMI_COMPILE_WARNINGS} -Wno-potentially-evaluated-expression")
        endif()
        if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
            set(STMMI_COMPILE_WARNINGS "${STMMI_COMPILE_WARNINGS} -ggdb")
            if (BUILD_WITH_SANITIZE)
                set(STMMI_COMPILE_WARNINGS "${STMMI_COMPILE_WARNINGS} -fsanitize=address")
            endif()
        endif()
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(STMMI_COMPILE_WARNINGS "${STMMI_COMPILE_WARNINGS} /W4 /WX /EHsc")
    endif()
    #set(CMAKE_CXX_FLAGS       "${CMAKE_CXX_FLAGS} ${STMMI_COMPILE_WARNINGS} -std=c++14"  PARENT_SCOPE)
    #
    string(REPLACE " " ";" REPLACED_FLAGS ${STMMI_COMPILE_WARNINGS})
    if (STMMI_INTERFACE_TYPE)
        target_compile_options(${STMMI_TARGET} INTERFACE ${REPLACED_FLAGS})
    else()
        target_compile_options(${STMMI_TARGET} PUBLIC ${REPLACED_FLAGS})
    endif()
endfunction(DefineTargetCompileOptionsType)
