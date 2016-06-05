# stmm-input-ev: test/CMakeLists.txt

# See   github.com/Barthelemy/CppProjectTemplate

include(CommonTesting)

option(BUILD_TESTING "Build tests" OFF)

# Test dirs
set(STMMI_TEST_SOURCES_DIR  "${PROJECT_SOURCE_DIR}/test")
# Test sources should end with .cxx
set(STMMI_TEST_SOURCES
        ${STMMI_TEST_SOURCES_DIR}/testEventClass.cxx
        ${STMMI_TEST_SOURCES_DIR}/testJoystickAxisEventClass.cxx
        ${STMMI_TEST_SOURCES_DIR}/testJoystickButtonEventClass.cxx
        ${STMMI_TEST_SOURCES_DIR}/testJoystickCapabilityClass.cxx
        ${STMMI_TEST_SOURCES_DIR}/testJoystickHatEventClass.cxx
        ${STMMI_TEST_SOURCES_DIR}/testKeyEventClass.cxx
        ${STMMI_TEST_SOURCES_DIR}/testPointerEventClass.cxx
        ${STMMI_TEST_SOURCES_DIR}/testPointerScrollEventClass.cxx
        ${STMMI_TEST_SOURCES_DIR}/testTouchEventClass.cxx
       )

TestFiles(stmm-input-ev TRUE "${STMMI_TEST_SOURCES}")

include(CTest)
