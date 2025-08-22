cmake_minimum_required(VERSION 3.15)
project(PathTracingRenderer)

# Add SDL3 as a subdirectory
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

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

# Add compiler flags for optimization and warnings
if(MSVC)
    target_compile_options(PathTracingRenderer PRIVATE /W4 /O2)
else()
    target_compile_options(PathTracingRenderer PRIVATE -Wall -Wextra -O3)
endif()

# Enable threading support
find_package(Threads REQUIRED)
target_link_libraries(PathTracingRenderer PRIVATE Threads::Threads)
target_link_libraries(PathTracingRenderer PRIVATE SDL3::SDL3)

# Copy SDL3.dll to the output directory
if(WIN32)
    add_custom_command(TARGET PathTracingRenderer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:PathTracingRenderer>
    )
endif()

# Copy model files to the output directory
add_custom_command(TARGET PathTracingRenderer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/models
    $<TARGET_FILE_DIR:PathTracingRenderer>/../models
)