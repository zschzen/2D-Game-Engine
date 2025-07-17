# --------------------------------------------------------------------
# Project Setup
# --------------------------------------------------------------------
cmake_minimum_required(VERSION 3.26)
project(CozerExamples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------------------------------------------------------------------
# Output Directories
# --------------------------------------------------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# --------------------------------------------------------------------
# Import Tools and Dependencies
# --------------------------------------------------------------------
include(../cmake/CPM.cmake)
include(../cmake/tools.cmake)

CPMAddPackage(NAME 2DEngine SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
CPMAddPackage("gh:TheLartians/GroupSourcesByFolder.cmake@1.0")

# --------------------------------------------------------------------
# Automatic Examples Discovery
# --------------------------------------------------------------------
file(GLOB_RECURSE example_main_files
    LIST_DIRECTORIES false
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*/main.cpp"
)

foreach(main_file IN LISTS example_main_files)
  # Extract example path components
  get_filename_component(example_abs_path ${main_file} DIRECTORY)
  file(RELATIVE_PATH example_rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${example_abs_path})

  # Generate clean target name (e.g., core_basic_window)
  string(REPLACE "/" "_" target_name ${example_rel_path})
  string(TOLOWER ${target_name} target_name)

  # Collect all source files in the example directory
  file(GLOB example_sources CONFIGURE_DEPENDS
        "${example_abs_path}/*.cpp"
        "${example_abs_path}/*.h"
    )

  # Create executable
  add_executable(${target_name} ${example_sources})

  # Set target properties
  set_target_properties(${target_name} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        OUTPUT_NAME "${target_name}"
        FOLDER "Examples"
    )

  # Include directories
  target_include_directories(${target_name} PRIVATE
        ${example_abs_path}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

  # Link dependencies
  target_link_libraries(${target_name} PRIVATE 2DEngine::2DEngine)

  # Platform-specific configurations
  if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_property(TARGET ${target_name} PROPERTY WIN32_EXECUTABLE true)
    target_link_options(${target_name} PRIVATE -static -static-libgcc -static-libstdc++)
  endif()

  if(EMSCRIPTEN)
    set_target_properties(${target_name}
            PROPERTIES
            SUFFIX ".html"
            OUTPUT_NAME "${target_name}"
        )
    target_link_options(${target_name} PRIVATE
      "SHELL:-s WASM=1"
      "SHELL:-s USE_SDL=2"
      "SHELL:-s ALLOW_MEMORY_GROWTH=1"
      "SHELL:-s ASSERTIONS=1"
      "SHELL:-s ASYNCIFY"
      #"SHELL:-s EXIT_RUNTIME=1"
    )

  endif()

  if(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    target_link_libraries(${target_name} PRIVATE ${COCOA_LIBRARY})
  endif()

  # Organize source files in IDE
  GroupSourcesByFolder(${target_name})
endforeach()

# --------------------------------------------------------------------
# Global Configuration
# --------------------------------------------------------------------
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
