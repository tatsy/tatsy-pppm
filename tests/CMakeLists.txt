# Enable GTest
enable_testing()

option(BUILD_TEST "BUILD_TEST" ON)
set(GTEST_ROOT $ENV{GTEST_ROOT} CACHE STRING "GTEST_ROOT")

if (${BUILD_TEST} STREQUAL "ON")
  # Link directory
  link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

  if (NOT "${GTEST_ROOT}" STREQUAL "")
    set(GTEST_INCLUDE_DIRS $ENV{GTEST_ROOT}/include)
    set(GTEST_LIBRARY_DIRS $ENV{GTEST_ROOT}/lib)
    if (WIN32)
      set(GTEST_LIBRARIES ${GTEST_LIBRARIES} "${LIB_PREFIX}gtestd${LIB_POSTFIX}" "${LIB_PREFIX}gtest_maind${LIB_POSTFIX}")
    else()
      set(GTEST_LIBRARIES ${GTEST_LIBRARIES} "${LIB_PREFIX}gtest${LIB_POSTFIX}" "${LIB_PREFIX}gtest_main${LIB_POSTFIX}")
    endif()
  endif()

  set(TEST_NAME unittests)
  set(SOURCE_FILES all_tests.cc
                   test_vector3d.cc
                   test_trimesh.cc)

  include_directories(${CMAKE_CURRENT_LIST_DIR})
  include_directories(${GTEST_INCLUDE_DIRS})
  link_directories(${GTEST_LIBRARY_DIRS})

  add_executable(${TEST_NAME} ${SOURCE_FILES})

  target_link_libraries(${TEST_NAME} ${GTEST_LIBRARIES})
  target_link_libraries(${TEST_NAME} "${LIB_PREFIX}tatsy_pppm${LIB_POSTFIX}")

  add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
  add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose DEPENDS ${TEST_NAME})
endif()
