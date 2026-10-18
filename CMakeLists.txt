# C:\Users\oscar\.cargo\bin\samply record .\build\tetris.exe
cmake_minimum_required(VERSION 3.15)
project(Tetris)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Point to MSYS2 SFML 3
list(APPEND CMAKE_PREFIX_PATH "C:/msys64/ucrt64")
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)
find_package(Boost REQUIRED CONFIG)

set(SOURCES
    src/main.cpp
    src/app/Game.cpp
    src/board/Bag.cpp
    src/board/Board.cpp
    src/guide/Guide.cpp
    src/guide/GuidePatterns.cpp
    src/input/InputThread.cpp
    src/piece/Pieces.cpp
    src/piece/Srs.cpp
    src/render/BoardBackground.cpp
    src/render/BoardRenderer.cpp
    src/render/DebugOverlay.cpp
    src/render/Histogram.cpp
    src/settings/SettingsUI.cpp
)

add_executable(Tetris ${SOURCES})

add_custom_command(TARGET Tetris POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<CONFIG>/assets
    COMMENT "Copying assets folder")

target_include_directories(Tetris PRIVATE src)
target_link_libraries(Tetris PRIVATE Boost::boost)
target_link_libraries(Tetris PRIVATE SFML::Graphics SFML::Window SFML::System)
