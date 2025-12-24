cmake_minimum_required(VERSION 3.20)
project(MicroRacingGame VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use vcpkg if available
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# Find SDL3 and dependencies
find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED CONFIG)

# Game executable
add_executable(racing_game
    src/main.cpp
    src/game.cpp
    src/game.h
    src/menu.cpp
    src/menu.h
    src/car.cpp
    src/car.h
    src/track.cpp
    src/track.h
    src/camera.cpp
    src/camera.h
    src/ai_bot.cpp
    src/ai_bot.h
    src/physics.cpp
    src/physics.h
    src/renderer.cpp
    src/renderer.h
    src/input.cpp
    src/input.h
)

target_link_libraries(racing_game PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    nlohmann_json::nlohmann_json
)

# Map editor executable
add_executable(map_editor
    src/editor_main.cpp
    src/editor.cpp
    src/editor.h
    src/track.cpp
    src/track.h
    src/renderer.cpp
    src/renderer.h
    src/input.cpp
    src/input.h
)

target_link_libraries(map_editor PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    nlohmann_json::nlohmann_json
)

# Copy assets to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/tracks DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Install targets
install(TARGETS racing_game map_editor
    RUNTIME DESTINATION bin
)
install(DIRECTORY assets tracks
    DESTINATION share/racing_game
)
