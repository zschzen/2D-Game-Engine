string(TIMESTAMP BEFORE "%s")
CPMAddPackage(
  NAME SPDLOG
  GITHUB_REPOSITORY gabime/spdlog
  VERSION 1.15.3
  OPTIONS
  "SPDLOG_INSTALL ON"
 )
if(SPDLOG_ADDED)
    list(APPEND ENGINE_LIBRARIES spdlog::spdlog_header_only)

    string(TIMESTAMP AFTER "%s")
    math(EXPR DELTA_SPDLOG "${AFTER} - ${BEFORE}")
    message(STATUS "SPDLOG time: ${DELTA_SPDLOG}s")
else()
    message(FATAL_ERROR "Failed to add SPDLOG package")
endif()
