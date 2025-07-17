# --------------------------------------------------------------------
# Global Properties
# --------------------------------------------------------------------
set(LUA_VERSION 5.4.8)

# --------------------------------------------------------------------
# Platform-Specific Definitions
# --------------------------------------------------------------------
if(UNIX AND NOT APPLE)
  set(LUA_PLATFORM_DEFINE LUA_USE_LINUX)
elseif(APPLE)
  set(LUA_PLATFORM_DEFINE LUA_USE_MACOSX)
elseif(WIN32)
  set(LUA_PLATFORM_DEFINE LUA_USE_WINDOWS)
else()
  set(LUA_PLATFORM_DEFINE LUA_ANSI)
endif()

# --------------------------------------------------------------------
# Lua Setup
# --------------------------------------------------------------------
string(TIMESTAMP BEFORE "%s")

# Add Lua
CPMAddPackage(
  NAME Lua
  GITHUB_REPOSITORY lua/lua
  VERSION ${LUA_VERSION}
  DOWNLOAD_ONLY YES
)

if(NOT Lua_ADDED)
  message(FATAL_ERROR "Failed to add Lua package")
endif()

# --------------------------------------------------------------------
# Lua Source Files
# --------------------------------------------------------------------
file(GLOB LUA_SOURCES "${Lua_SOURCE_DIR}/*.c")
list(REMOVE_ITEM LUA_SOURCES "${lua_SOURCE_DIR}/lua.c" "${lua_SOURCE_DIR}/luac.c")

# --------------------------------------------------------------------
# Lua Library
# --------------------------------------------------------------------
add_library(lua STATIC ${LUA_SOURCES})

target_compile_definitions(lua PUBLIC ${LUA_PLATFORM_DEFINE})

target_include_directories(lua SYSTEM PUBLIC $<BUILD_INTERFACE:${Lua_SOURCE_DIR}>)

# --------------------------------------------------------------------
# Package Lua
# --------------------------------------------------------------------
packageProject(
  NAME lua
  VERSION ${LUA_VERSION}
  NAMESPACE lua
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${Lua_SOURCE_DIR}
  COMPATIBILITY SameMajorVersion
)

# --------------------------------------------------------------------
# SOL2 Dependency
# --------------------------------------------------------------------
string(TIMESTAMP BEFORE_SOL2 "%s")

CPMAddPackage(
  NAME sol2
  GITHUB_REPOSITORY ThePhD/sol2
  GIT_TAG v3.5.0
  OPTIONS
    "SOL2_BUILD_LUA OFF"
    "SOL2_ENABLE_INSTALL ON"
    "SOL2_TESTS OFF"
    "SOL2_EXAMPLES OFF"
)

if(sol2_ADDED)
  # --------------------------------------------------------------------
  # Include & Library Directories
  # --------------------------------------------------------------------
  list(APPEND ENGINE_LIBRARIES_INCLUDE_DIR
    ${Lua_SOURCE_DIR}
    ${sol2_SOURCE_DIR}/include/sol
  )

  list(APPEND ENGINE_LIBRARIES
    sol2::sol2
    lua
  )

  # --------------------------------------------------------------------
  # Timing
  # --------------------------------------------------------------------
  string(TIMESTAMP AFTER_SOL2 "%s")
  math(EXPR DELTA_SOL2 "${AFTER_SOL2} - ${BEFORE_SOL2}")
  message(STATUS "sol2 time: ${DELTA_SOL2}s")
else()
  message(FATAL_ERROR "Failed to add sol2 package")
endif()

