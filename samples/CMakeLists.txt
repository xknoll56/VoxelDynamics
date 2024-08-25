cmake_minimum_required(VERSION 3.5.0)

project(VoxelDynamics)

# Set platform-specific paths
if(WIN32)
    set(GLFW3_INCLUDE "C:/path/to/glfw/include" CACHE PATH "Path to GLFW include")
    set(GLFW3_LIB "C:/path/to/glfw/lib" CACHE PATH "Path to GLFW lib")
    set(GLEW_INCLUDE "C:/path/to/glew/include" CACHE PATH "Path to GLEW include")
    set(GLEW_LIB "C:/path/to/glew/lib" CACHE PATH "Path to GLEW lib")
    set(OPENGL_HINTS "C:/path/to/opengl" CACHE PATH "Path to OpenGL installation")
endif()


# Include the subdirectories containing the CMakeLists.txt files
add_subdirectory(HelloVoxelDynamics)
add_subdirectory(HelloBody)