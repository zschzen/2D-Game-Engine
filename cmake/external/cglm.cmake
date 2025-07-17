string(TIMESTAMP BEFORE "%s")
CPMAddPackage(
  NAME CGLM
  GITHUB_REPOSITORY recp/cglm
  VERSION 0.9.6
  OPTIONS
)
if(CGLM_ADDED)
  list(APPEND ENGINE_LIBRARIES_INCLUDE_DIR ${CGLM_SOURCE_DIR}/include)
  list(APPEND ENGINE_LIBRARIES cglm)

  string(TIMESTAMP AFTER "%s")
  math(EXPR DELTA_SDL "${AFTER} - ${BEFORE}")
  message(STATUS "CGLM time: ${DELTA_SDL}s")
else()
  message(FATAL_ERROR "Failed to add CGLM package")
endif()

