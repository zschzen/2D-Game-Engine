#--------------------------------------------------------------------
# Source and Header Files
#--------------------------------------------------------------------
set(ENGINE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include/engine)
set(ENGINE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

# Header files
set(ENGINE_HEADER_FILES
    ${ENGINE_INCLUDE_DIR}/engine.hpp
)

set(ENGINE_SOURCE_FILES
    ${ENGINE_SOURCE_DIR}/engine.cpp
)

#--------------------------------------------------------------------
# Library Setup
#--------------------------------------------------------------------
add_library(${PROJECT_NAME} ${ENGINE_SOURCE_FILES} ${ENGINE_HEADER_FILES})
target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
  PRIVATE
    ${ENGINE_LIBRARIES_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Enforce standards conformance on MSVC
target_compile_options(${PROJECT_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")

# Generates a partially processed version of main header
# target_precompile_headers(${PROJECT_NAME} PRIVATE ${ENGINE_INCLUDE_DIR}/engine.hpp)

target_link_libraries(${PROJECT_NAME} PUBLIC ${ENGINE_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED YES
  CXX_EXTENSIONS NO
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

#--------------------------------------------------------------------
# Compile definitions
#--------------------------------------------------------------------
target_compile_definitions(${PROJECT_NAME} PUBLIC
    # Shared
    $<$<BOOL:${BUILD_SHARED_LIBS}>:EN_SHARED>

    # Build Type
    $<$<CONFIG:Debug>:EN_DEBUG>
    $<$<CONFIG:Release>:EN_RELEASE>

    # Backend
    "${PLATFORM_BACKEND}"

    # Log/Debug
    $<$<BOOL:${LOG_SUPPORT}>:LOG_SUPPORT>
)

#--------------------------------------------------------------------
# Source Groups
#--------------------------------------------------------------------
GroupSourcesByFolder(${PROJECT_NAME})
