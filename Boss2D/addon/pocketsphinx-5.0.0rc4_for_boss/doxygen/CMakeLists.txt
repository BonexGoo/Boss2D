find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(DOXYGEN_PROJECT_NUMBER 5.0.0rc4)
  set(DOXYGEN_EXAMPLE_PATH ${CMAKE_SOURCE_DIR}/examples)
  set(DOXYGEN_EXCLUDE_PATTERNS
    *export.h
    *config.h
    )
  doxygen_add_docs(docs
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/include)
endif()
install(FILES
  pocketsphinx.1
  pocketsphinx_batch.1
  pocketsphinx_mdef_convert.1
  sphinx_lm_convert.1
  sphinx_lm_eval.1
  sphinx_lm_sort.1
  sphinx_pitch.1
  DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
