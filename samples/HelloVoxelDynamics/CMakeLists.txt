cmake_minimum_required(VERSION 3.5.0)
project(HelloVoxelDynamics VERSION 0.1.0 LANGUAGES C CXX)

# Add the include directory
include_directories(${CMAKE_SOURCE_DIR}/../../include)
include_directories(${CMAKE_SOURCE_DIR}/../Common)


# Set platform-specific paths
if(WIN32)
    # Allow setting these paths via the CMake GUI or command line
    set(GLFW3_INCLUDE "C:/path/to/glfw/include" CACHE PATH "Path to GLFW include")
    set(GLFW3_LIB "C:/path/to/glfw/lib" CACHE PATH "Path to GLFW lib")
    set(GLEW_INCLUDE "C:/path/to/glew/include" CACHE PATH "Path to GLEW include")
    set(GLEW_LIB "C:/path/to/glew/lib" CACHE PATH "Path to GLEW lib")
    set(OPENGL_HINTS "C:/path/to/opengl" CACHE PATH "Path to OpenGL installation")

    # Define GLEW_STATIC for Windows
    add_definitions(-DGLEW_STATIC)
endif()

# Try to find GLFW
find_package(GLFW3 QUIET HINTS ${GLFW3_HINTS})
if(NOT GLFW3_FOUND)
    message(WARNING "GLFW3 not found. Setting paths manually.")
    if(WIN32)
        set(GLFW3_INCLUDE_DIR "${GLFW3_INCLUDE}")
        set(GLFW3_LIBRARY "${GLFW3_LIB}/glfw3.lib")
    else()
        set(GLFW3_INCLUDE_DIR "/usr/include")
        set(GLFW3_LIBRARY "/usr/lib/x86_64-linux-gnu/libglfw.so")
    endif()
    include_directories(${GLFW3_INCLUDE_DIR})
else()
    include_directories(${GLFW_INCLUDE_DIRS})
endif()

# Try to find GLEW
find_package(GLEW QUIET HINTS ${GLEW_HINTS})
if(NOT GLEW_FOUND)
    message(WARNING "GLEW not found. Setting paths manually.")
    if(WIN32)
        set(GLEW_INCLUDE_DIR "${GLEW_INCLUDE}")
        set(GLEW_LIBRARY "${GLEW_LIB}/glew32s.lib")
    else()
        set(GLEW_INCLUDE_DIR "/usr/include")
        set(GLEW_LIBRARY "/usr/lib/x86_64-linux-gnu/libGLEW.so")
    endif()
    include_directories(${GLEW_INCLUDE_DIR})
else()
    include_directories(${GLEW_INCLUDE_DIRS})
endif()

# Try to find OpenGL
find_package(OpenGL REQUIRED)
if(NOT OpenGL_FOUND)
    message(WARNING "OpenGL not found. Setting paths manually.")
    if(WIN32)
        set(OPENGL_INCLUDE_DIR "${OPENGL_HINTS}/include")
        set(OPENGL_LIBRARY "${OPENGL_HINTS}/lib/OpenGL32.lib")
    else()
        set(OPENGL_INCLUDE_DIR "/usr/include")
        set(OPENGL_LIBRARY "/usr/lib/x86_64-linux-gnu/libGL.so")
    endif()
    include_directories(${OPENGL_INCLUDE_DIR})
else()
    include_directories(${OPENGL_INCLUDE_DIR})
endif()

# Custom command to copy shaders
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/Shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/../Common/Shaders ${CMAKE_BINARY_DIR}/Shaders
    DEPENDS ${CMAKE_SOURCE_DIR}/../Common/Shaders
    COMMENT "Copying shaders..."
)

# Add the executable
add_executable(HelloVoxelDynamics main.cpp)

# Ensure the shaders are copied before building the executable
add_dependencies(HelloVoxelDynamics ${CMAKE_BINARY_DIR}/Shaders)

# Link the libraries to the executable
if(GLFW3_FOUND)
    target_link_libraries(HelloVoxelDynamics ${GLFW_LIBRARIES})
else()
    target_link_libraries(HelloVoxelDynamics ${GLFW3_LIBRARY})
endif()

if(GLEW_FOUND)
    target_link_libraries(HelloVoxelDynamics ${GLEW_LIBRARIES})
else()
    target_link_libraries(HelloVoxelDynamics ${GLEW_LIBRARY})
endif()

if(OpenGL_FOUND)
    target_link_libraries(HelloVoxelDynamics ${OPENGL_LIBRARIES})
else()
    target_link_libraries(HelloVoxelDynamics ${OPENGL_LIBRARY})
endif()
