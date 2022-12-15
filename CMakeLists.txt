cmake_minimum_required(VERSION 3.25)
project(ChristmasGame)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/raylib)

add_executable(game 
    src/main.cpp 
    src/Application.cpp
    src/Textures.cpp
    src/PixelFont.cpp
    src/Transitions.cpp
)

target_precompile_headers(game PUBLIC src/include/pch.h)
target_link_libraries(game PRIVATE raylib)
target_include_directories(game PRIVATE src/include)
add_compile_options(-Wall -std=c++17)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)

add_dependencies(game copy_assets)

# CMAKE_BUILD_TYPE(RELEASE)

if (EMSCRIPTEN) # emcmake cmake .. -DPLATFORM=Web && cmake --build .
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -std=c++17 -s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY --preload-file assets --shell-file C:/raylib/raylib/src/customshell.html")
    set(CMAKE_EXECUTABLE_SUFFIX ".html") # This line is used to set your executable to build with the emscripten html template so that you can directly open it.
endif ()
