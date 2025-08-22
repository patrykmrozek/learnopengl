cmake_minimum_required(VERSION 3.10)
project(learnopengl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
set(GLFW_ROOT /usr/local/opt/glfw)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Prevent GLFW from including OpenGL headers
add_definitions(-DGLFW_INCLUDE_NONE)

# Create a static library with common OpenGL setup
add_library(learnopengl_lib STATIC
    libs/glad/glad.c
)

# Include directories for the library (PUBLIC so executables inherit them)
target_include_directories(learnopengl_lib PUBLIC
    include/
)

# Link libraries to the library target
target_link_libraries(learnopengl_lib PUBLIC 
    glfw
    ${OPENGL_LIBRARIES}
)

# Platform-specific libraries for macOS
if(APPLE)
    target_link_libraries(learnopengl_lib PUBLIC
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()

# Function to create OpenGL executables easily
function(add_opengl_exec name source)
    add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE learnopengl_lib)
endfunction()

# Create executables using the function
add_opengl_exec(triangle src/triangle.cpp)
add_opengl_exec(square src/square.cpp)

