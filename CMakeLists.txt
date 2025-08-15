cmake_minimum_required(VERSION 3.15)
project(PathTracingRenderer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(src)

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
target_link_libraries(PathTracingRenderer Threads::Threads)
