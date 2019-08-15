if(NOT HAVE_XINE AND PKG_CONFIG_FOUND)
  ocv_check_modules(XINE libxine QUIET)
endif()

if(HAVE_XINE)
  ocv_add_external_target(xine "${XINE_INCLUDE_DIRS}" "${XINE_LIBRARIES}" "HAVE_XINE")
endif()

set(HAVE_XINE ${HAVE_XINE} PARENT_SCOPE)
