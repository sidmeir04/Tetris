cmake_minimum_required(VERSION 3.15)
project(Tetris)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Point to MSYS2 SFML 3
list(APPEND CMAKE_PREFIX_PATH "C:/msys64/ucrt64")
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

# 2. Automatically find all .cpp files in the src directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 3. Create the executable using the found source files
add_executable(Tetris ${SOURCES})

# Copy assets folder to build directory
add_custom_command(TARGET Tetris POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<CONFIG>/assets
    COMMENT "Copying assets folder")
    
# 4. Tell the compiler where the .hpp files are 
# (This allows you to write #include "Game.hpp" without paths)
target_include_directories(Tetris PRIVATE headers)

# 5. Link SFML
target_link_libraries(Tetris PRIVATE SFML::Graphics SFML::Window SFML::System)