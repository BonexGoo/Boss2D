set(TEST_EXECUTABLES
  test_fsg_read
  test_fsg_jsgf
  test_fsg_write_fsm
  )
foreach(TEST_EXECUTABLE ${TEST_EXECUTABLES})
  add_executable(${TEST_EXECUTABLE} EXCLUDE_FROM_ALL ${TEST_EXECUTABLE}.c)
  target_link_libraries(${TEST_EXECUTABLE} pocketsphinx)
  target_include_directories(
    ${TEST_EXECUTABLE} PRIVATE ${CMAKE_SOURCE_DIR}/src
    ${TEST_EXECUTABLE} PRIVATE ${CMAKE_BINARY_DIR}
    ${TEST_EXECUTABLE} PRIVATE ${CMAKE_BINARY_DIR}/test/unit
    ${TEST_EXECUTABLE} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
    )
  add_test(NAME ${TEST_EXECUTABLE} COMMAND ${TEST_EXECUTABLE})
  set_property(TARGET ${TEST_EXECUTABLE}
    PROPERTY COMPILE_DEFINITIONS LMDIR="${CMAKE_CURRENT_SOURCE_DIR}")
  add_dependencies(check ${TEST_EXECUTABLE})
endforeach()
