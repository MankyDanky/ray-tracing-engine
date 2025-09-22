cmake_minimum_required(VERSION 3.15)
project(PathTracingRenderer)

# Check if building with Emscripten
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    # Removed USE_PTHREADS to avoid SharedArrayBuffer issues
    set(USE_FLAGS "-s USE_SDL=2 -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s EXPORTED_RUNTIME_METHODS=['cwrap'] -s ASSERTIONS=1 -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=2 -s GL_ENABLE_GET_PROC_ADDRESS=1")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
else()
    # Original SDL setup for native builds
    add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(
    src
    src/core
    src/geometry
    src/scene
    src/renderer
    src/materials
    src/utils
)

# Add source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(PathTracingRenderer ${SOURCES})

# Add compiler flags
if(MSVC)
    target_compile_options(PathTracingRenderer PRIVATE /W4)
elseif(EMSCRIPTEN)
    # Emscripten-specific optimizations
    target_compile_options(PathTracingRenderer PRIVATE -O3)
else()
    target_compile_options(PathTracingRenderer PRIVATE -Wall -Wextra -O3)
endif()

# Link libraries
if(EMSCRIPTEN)
    # Emscripten handles SDL linking automatically with USE_SDL flag
else()
    find_package(Threads REQUIRED)
    target_link_libraries(PathTracingRenderer PRIVATE Threads::Threads)
    target_link_libraries(PathTracingRenderer PRIVATE SDL3::SDL3)
endif()

# Copy assets for web build
if(EMSCRIPTEN)
    add_custom_command(TARGET PathTracingRenderer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/build/models
        ${CMAKE_CURRENT_BINARY_DIR}/models
    )
endif()