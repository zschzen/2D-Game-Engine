if(NOT DEFINED EMSCRIPTEN)

    # ----------------------------------------------------------------
    # Global Build Options
    # ----------------------------------------------------------------
    set(BUILD_SHARED_LIBS ON)

    set(SDL2_DISABLE_INSTALL OFF CACHE BOOL "Enable SDL2 installation" FORCE)
    set(SDL_SHARED ON CACHE BOOL "Build SDL2 as shared library" FORCE)
    set(SDL_STATIC OFF CACHE BOOL "Disable SDL2 static library" FORCE)
    set(SDL_WERROR OFF CACHE BOOL "Disable warnings as errors in SDL2" FORCE)

    # ----------------------------------------------------------------
    # CMake policy / language support
    # ----------------------------------------------------------------
    # Allows to configure projects w/ CMAKE < 3.5 in systems w/ >= 4.0
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

    # Enable Assembly. Mostly for SDL_image
    enable_language(ASM-ATT)

    # ----------------------------------------------------------------
    # SDL2
    # ----------------------------------------------------------------
    string(TIMESTAMP BEFORE_SDL2 "%s")
    CPMAddPackage(
        NAME SDL2
        GITHUB_REPOSITORY libsdl-org/SDL
        GIT_TAG release-2.32.8
        OPTIONS
            "SDL2_DISABLE_INSTALL OFF"
            "SDL_SHARED ON"
            "SDL_STATIC OFF"
            "SDL_WERROR OFF"
    )

    if(NOT SDL2_ADDED)
        message(FATAL_ERROR "Failed to fetch SDL2 from repository")
    endif()

    find_package(SDL2 REQUIRED CONFIG)
    string(TIMESTAMP AFTER_SDL2 "%s")
    math(EXPR DELTA_SDL2 "${AFTER_SDL2} - ${BEFORE_SDL2}")
    message(STATUS "SDL2 TIME: ${DELTA_SDL2}s")

    # ----------------------------------------------------------------
    # SDL2_image
    # ----------------------------------------------------------------
    string(TIMESTAMP BEFORE_IMAGE "%s")
    CPMAddPackage(
        NAME SDL2_image
        GITHUB_REPOSITORY libsdl-org/SDL_image
        GIT_TAG release-2.8.8
        OPTIONS
            "SDL2IMAGE_INSTALL ON"
            "SDL2IMAGE_SAMPLES OFF"
            "SDL2IMAGE_VENDORED ON"
            "SDL2IMAGE_BUILD_SHARED_LIBS ON"
            "BUILD_SHARED_LIBS ON"
    )

    find_package(SDL2_image REQUIRED)
    string(TIMESTAMP AFTER_IMAGE "%s")
    math(EXPR DELTA_IMAGE "${AFTER_IMAGE} - ${BEFORE_IMAGE}")
    message(STATUS "SDL2_image TIME: ${DELTA_IMAGE}s")

    # ----------------------------------------------------------------
    # SDL2_ttf
    # ----------------------------------------------------------------
    string(TIMESTAMP BEFORE_TTF "%s")
    CPMAddPackage(
        NAME SDL2_ttf
        GITHUB_REPOSITORY libsdl-org/SDL_ttf
        GIT_TAG release-2.24.0
        OPTIONS
            "SDL2TTF_INSTALL ON"
            "SDL2TTF_BUILD_SHARED_LIBS ON"
            "SDL2TTF_VENDORED ON"
            "SDL2TTF_SAMPLES OFF"
    )

    find_package(SDL2_ttf REQUIRED)
    string(TIMESTAMP AFTER_TTF "%s")
    math(EXPR DELTA_TTF "${AFTER_TTF} - ${BEFORE_TTF}")
    message(STATUS "SDL2_ttf TIME: ${DELTA_TTF}s")

    # ----------------------------------------------------------------
    # SDL2_mixer
    # ----------------------------------------------------------------
    string(TIMESTAMP BEFORE_MIXER "%s")
    CPMAddPackage(
        NAME SDL2_mixer
        GITHUB_REPOSITORY libsdl-org/SDL_mixer
        GIT_TAG release-2.8.1
        OPTIONS
            "SDL2MIXER_INSTALL ON"
            "SDL2MIXER_VENDORED ON"
            "SDL2MIXER_SAMPLES OFF"
            "SDL2MIXER_BUILD_SHARED_LIBS ON"
    )

    find_package(SDL2_mixer REQUIRED)
    string(TIMESTAMP AFTER_MIXER "%s")
    math(EXPR DELTA_MIXER "${AFTER_MIXER} - ${BEFORE_MIXER}")
    message(STATUS "SDL2_mixer TIME: ${DELTA_MIXER}s")

    # ----------------------------------------------------------------
    # Engine include directories
    # ----------------------------------------------------------------
    list(APPEND ENGINE_LIBRARIES_INCLUDE_DIR
        "${SDL2_SOURCE_DIR}/include"
        "${SDL2_image_SOURCE_DIR}/include"
        "${SDL2_ttf_SOURCE_DIR}"
        "${SDL2_mixer_SOURCE_DIR}/include"
    )

    # ----------------------------------------------------------------
    # Engine linkable libraries (targets)
    # ----------------------------------------------------------------
    list(APPEND ENGINE_LIBRARIES
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
        $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
        $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    )

    # ----------------------------------------------------------------
    # Summary
    # ----------------------------------------------------------------
    message(STATUS "SDL2 and extensions configured")
endif()

