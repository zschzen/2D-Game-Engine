# Define ImGui version
set(IMGUI_VERSION 1.92.1)

# Record the start time for ImGui setup
string(TIMESTAMP BEFORE "%s")

# Add ImGui
CPMAddPackage(
  NAME imgui
  GITHUB_REPOSITORY ocornut/imgui
  GIT_TAG v${IMGUI_VERSION}
  DOWNLOAD_ONLY YES
)

if(imgui_ADDED)
  # Add ImGui source files
  set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
  )

  # Include ImGui directories
  list(APPEND ENGINE_LIBRARIES_INCLUDE_DIR
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
  )

  # Add ImGui to the project
  add_library(imgui STATIC ${IMGUI_SOURCES})
  target_include_directories(imgui PUBLIC
    "${SDL2_SOURCE_DIR}/include"
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
  )
  target_link_libraries(imgui PUBLIC ${SDL2_LIBRARIES})

  # Package ImGui
  packageProject(
    NAME imgui
    VERSION ${IMGUI_VERSION}
    NAMESPACE imgui
    BINARY_DIR ${PROJECT_BINARY_DIR}
    INCLUDE_DIR ${imgui_SOURCE_DIR}
    COMPATIBILITY SameMajorVersion
  )

  set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)

  list(APPEND ENGINE_LIBRARIES imgui)

  # Record the end time and calculate the duration
  string(TIMESTAMP AFTER "%s")
  math(EXPR DELTA_IMGUI "${AFTER} - ${BEFORE}")
  message(STATUS "ImGui setup time: ${DELTA_IMGUI}s")
else()
  message(FATAL_ERROR "Failed to add ImGui package")
endif()

