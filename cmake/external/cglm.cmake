# ----------------------------------------------------------------
# CGLM
# ----------------------------------------------------------------
string(TIMESTAMP BEFORE_CGLM "%s")

find_package(cglm QUIET CONFIG)

if(cglm_FOUND)
    message(STATUS "Using system CGLM")
    list(APPEND ENGINE_LIBRARIES cglm::cglm)
else()
    message(STATUS "CGLM not found in system, fetching and compiling from repository...")
    CPMAddPackage(
            NAME CGLM
            GITHUB_REPOSITORY recp/cglm
            VERSION 0.9.6
            OPTIONS
        )

    if(CGLM_ADDED)
        list(APPEND ENGINE_LIBRARIES_INCLUDE_DIR ${CGLM_SOURCE_DIR}/include)
        list(APPEND ENGINE_LIBRARIES cglm)
    else()
        message(FATAL_ERROR "Failed to add CGLM package")
    endif()
endif()

string(TIMESTAMP AFTER_CGLM "%s")
math(EXPR DELTA_CGLM "${AFTER_CGLM} - ${BEFORE_CGLM}")
message(STATUS "CGLM TIME: ${DELTA_CGLM}s")
