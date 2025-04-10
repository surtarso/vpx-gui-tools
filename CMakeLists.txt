cmake_minimum_required(VERSION 3.10)
project(VPXGUITools VERSION 0.9.9 LANGUAGES CXX C)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate a version.h header with the project version
configure_file(
    ${CMAKE_SOURCE_DIR}/version.h.in
    ${CMAKE_BINARY_DIR}/version.h
    @ONLY
)

# Define source files
set(SOURCES
    src/main.cpp
    src/core/application.cpp
    src/config/config_manager.cpp
    src/config/media_config.cpp
    src/config/path_config.cpp
    src/config/tools_config.cpp
    src/config/window_config.cpp
    src/tools/ini_editor.cpp
    src/tools/first_run.cpp
    src/launcher/launcher.cpp
    src/launcher/table_actions.cpp
    src/launcher/table_view.cpp
    src/tables/table_filter.cpp
    src/tables/table_loader.cpp
    src/tables/table_manager.cpp
    src/tables/table_updater.cpp
    # external submodules
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_sdl2.cpp
    external/imgui/backends/imgui_impl_sdlrenderer2.cpp
    # Add ImGuiFileDialog
    external/ImGuiFileDialog/ImGuiFileDialog.cpp
)

# Define the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/config
    ${CMAKE_SOURCE_DIR}/src/launcher
    ${CMAKE_SOURCE_DIR}/src/tools
    ${CMAKE_SOURCE_DIR}/src/tables
    ${CMAKE_SOURCE_DIR}/src/utils
    # external submodules
    ${CMAKE_SOURCE_DIR}/external/imgui
    ${CMAKE_SOURCE_DIR}/external/imgui/backends
    ${CMAKE_SOURCE_DIR}/external/ImGuiFileDialog
    ${CMAKE_SOURCE_DIR}/external/json/single_include/nlohmann
    ${CMAKE_BINARY_DIR}  # Where version.h is generated
)

# Find and link dependencies
find_package(SDL2 REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2::SDL2
)

# Compile options
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -D_REENTRANT
)

# Build type-specific flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG_LOGGING)
    target_compile_options(${PROJECT_NAME} PRIVATE -g)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE -O2)
endif()

# Suppress warnings for external submodules only
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set_source_files_properties(
        ${CMAKE_SOURCE_DIR}/external/ImGuiFileDialog/ImGuiFileDialog.cpp
        PROPERTIES
        COMPILE_FLAGS "-w"
    )
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Copy resources to the build directory after building the executable using a custom script
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        -D "SRC_DIR=${CMAKE_SOURCE_DIR}"
        -D "BIN_DIR=${CMAKE_BINARY_DIR}"
        -P "${CMAKE_SOURCE_DIR}/resources.cmake"
    COMMENT "Copying resources to build directory..."
)