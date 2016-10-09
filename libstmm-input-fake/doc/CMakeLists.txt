# stmm-input-fake: doc/CMakeLists.txt

# Modified from Barthélémy von Haller's  github.com/Barthelemy/CppProjectTemplate

include(FindDoxygen)

if (DOXYGEN_FOUND)
    if (NOT DOXYGEN_DOT_FOUND) 
        message(WARNING "Graphviz doesn't seem to be installed. Doxygen will not be able to generate graphs. Consider installing this package.")
    endif(NOT DOXYGEN_DOT_FOUND) 

    include(CommonDoxy)
    CreateLibDoxy(stmm-input-fake  ${STMM_INPUT_FAKE_VERSION}  "stmm-input-ev;stmm-input-base;stmm-input")
endif(DOXYGEN_FOUND)
