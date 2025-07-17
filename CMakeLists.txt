cmake_minimum_required(VERSION 4.0 FATAL_ERROR)
project(2DEngine
        VERSION 0.0.1
        DESCRIPTION "A simple 2D game engine developed following Pikuma's course"
        LANGUAGES C CXX
)

set(ENGINE_IS_MAIN ${PROJECT_IS_TOP_LEVEL})

# --------------------------------------------------------------------
# Paths
# --------------------------------------------------------------------
# Set CMake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

# --------------------------------------------------------------------
# Include External CMake Files
# --------------------------------------------------------------------
include(CMakeOptions.txt)

# --------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------
include(cmake/CPM.cmake)
include(cmake/tools.cmake)
CPMUsePackageLock(package-lock.cmake)

CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.13.0")
CPMAddPackage("gh:TheLartians/GroupSourcesByFolder.cmake@1.0")

list(APPEND ENGINE_LIBRARIES_INCLUDE_DIR)
list(APPEND ENGINE_LIBRARIES)

include(cmake/external/lua.cmake)
include(cmake/external/sdl.cmake)
include(cmake/external/imgui.cmake)
include(cmake/external/cglm.cmake)

# --------------------------------------------------------------------
# Output Directories
# --------------------------------------------------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# --------------------------------------------------------------------
# Global Properties
# --------------------------------------------------------------------
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# --------------------------------------------------------------------
# Subdirectories
# --------------------------------------------------------------------
add_subdirectory(src 2dengine)

# --------------------------------------------------------------------
# Installation Configuration
# --------------------------------------------------------------------
string(TOLOWER ${PROJECT_NAME}/version.h VERSION_HEADER_LOCATION)
string(TOLOWER ${PROJECT_NAME}/export.h EXPORT_HEADER_LOCATION)

packageProject(
  NAME ${PROJECT_NAME}
  VERSION ${PROJECT_VERSION}
  NAMESPACE ${PROJECT_NAME}
  BINARY_DIR ${PROJECT_BINARY_DIR}
  INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
  INCLUDE_DESTINATION include/${PROJECT_NAME}-${PROJECT_VERSION}
  VERSION_HEADER "${VERSION_HEADER_LOCATION}"
  EXPORT_HEADER "${EXPORT_HEADER_LOCATION}"
  COMPATIBILITY SameMajorVersion
)
