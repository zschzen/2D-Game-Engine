if (NOT DEFINED EMSCRIPTEN)
  string(TIMESTAMP BEFORE "%s")
  CPMAddPackage(
    NAME SDL2
    GITHUB_REPOSITORY libsdl-org/SDL
    GIT_TAG release-2.32.8
    OPTIONS
      "SDL_SHARED ON"
      "SDL_STATIC OFF"
      "SDL_TEST OFF"
      "SDL2_DISABLE_INSTALL OFF"
  )
  if(SDL2_ADDED)
    list(APPEND ENGINE_LIBRARIES_INCLUDE_DIR ${SDL2_SOURCE_DIR}/include)
    if(TARGET SDL2::SDL2main)
      list(APPEND ENGINE_LIBRARIES SDL2::SDL2main)
    endif()
    list(APPEND ENGINE_LIBRARIES SDL2::SDL2)
    string(TIMESTAMP AFTER "%s")
    math(EXPR DELTA_SDL "${AFTER} - ${BEFORE}")
    message(STATUS "SDL2 time: ${DELTA_SDL}s")
  else()
    message(FATAL_ERROR "Failed to add SDL2 package")
  endif()
else()
  # Emscripten-specific flags
  set(EMSCRIPTEN_FLAGS "-s USE_SDL=2")

  # Append flags to compiler/linker settings
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EMSCRIPTEN_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EMSCRIPTEN_FLAGS}")

endif()
