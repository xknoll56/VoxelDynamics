cmake_minimum_required(VERSION 3.5.0)
project(HelloVoxelDynamics VERSION 0.1.0 LANGUAGES C CXX)

# Add the include directory
include_directories(${CMAKE_SOURCE_DIR}/../../include)
include_directories(${CMAKE_SOURCE_DIR}/../Common)

# Copy shaders to the build directory
file(COPY ${CMAKE_SOURCE_DIR}/../Common/Shaders DESTINATION ${CMAKE_BINARY_DIR})

# Try to find GLFW
find_package(GLFW3 QUIET)
if(NOT GLFW3_FOUND)
    # Manually set GLFW paths if not found
    message(WARNING "GLFW3 not found. Setting paths manually.")
    set(GLFW3_INCLUDE_DIR "/usr/include/GLFW")
    set(GLFW3_LIBRARY "/usr/lib/x86_64-linux-gnu/libglfw.so")
    include_directories(${GLFW3_INCLUDE_DIR})
else()
    include_directories(${GLFW_INCLUDE_DIRS})
endif()

# Try to find GLEW
find_package(GLEW QUIET)
if(NOT GLEW_FOUND)
    # Manually set GLEW paths if not found
    message(WARNING "GLEW not found. Setting paths manually.")
    set(GLEW_INCLUDE_DIR "/usr/include")
    set(GLEW_LIBRARY "/usr/lib/x86_64-linux-gnu/libGLEW.so")
    include_directories(${GLEW_INCLUDE_DIR})
else()
    include_directories(${GLEW_INCLUDE_DIRS})
endif()

# Try to find OpenGL
find_package(OpenGL REQUIRED)
if(NOT OpenGL_FOUND)
    # Manually set OpenGL paths if not found
    message(WARNING "OpenGL not found. Setting paths manually.")
    set(OPENGL_INCLUDE_DIR "/usr/include")
    set(OPENGL_LIBRARY "/usr/lib/x86_64-linux-gnu/libGL.so")
    include_directories(${OPENGL_INCLUDE_DIR})
else()
    include_directories(${OPENGL_INCLUDE_DIR})
endif()

# Add the executable
add_executable(HelloVoxelDynamics main.cpp)

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
