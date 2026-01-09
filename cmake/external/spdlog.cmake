# ----------------------------------------------------------------
# SPDLOG
# ----------------------------------------------------------------
string(TIMESTAMP BEFORE_SPDLOG "%s")

find_package(spdlog QUIET CONFIG)

if(spdlog_FOUND)
    message(STATUS "Using system SPDLOG")
    list(APPEND ENGINE_LIBRARIES spdlog::spdlog_header_only)
else()
    message(STATUS "SPDLOG not found in system, fetching and compiling from repository...")
    CPMAddPackage(
            NAME SPDLOG
            GITHUB_REPOSITORY gabime/spdlog
            VERSION 1.17.0
            OPTIONS
                "SPDLOG_INSTALL ON"
        )

    if(SPDLOG_ADDED)
        list(APPEND ENGINE_LIBRARIES spdlog::spdlog_header_only)
    else()
        message(FATAL_ERROR "Failed to add SPDLOG package")
    endif()
endif()

string(TIMESTAMP AFTER_SPDLOG "%s")
math(EXPR DELTA_SPDLOG "${AFTER_SPDLOG} - ${BEFORE_SPDLOG}")
message(STATUS "SPDLOG TIME: ${DELTA_SPDLOG}s")
