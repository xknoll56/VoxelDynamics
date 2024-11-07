cmake_minimum_required(VERSION 3.5.0)
project(HelloTriangle VERSION 0.1.0 LANGUAGES C CXX)

# Add the executable
add_executable(HelloTriangle main.cpp)

# Custom target to copy shaders to the current project's binary directory
add_custom_command(TARGET HelloTriangle POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Common/Shaders ${CMAKE_CURRENT_BINARY_DIR}/Shaders
    COMMENT "Copying shaders to current project's binary directory..."
)

# Link the libraries to the executable
if(GLFW3_FOUND)
    target_link_libraries(HelloTriangle ${GLFW_LIBRARIES})
else()
    target_link_libraries(HelloTriangle ${GLFW3_LIBRARY})
endif()

if(GLEW_FOUND)
    target_link_libraries(HelloTriangle ${GLEW_LIBRARIES})
else()
    target_link_libraries(HelloTriangle ${GLEW_LIBRARY})
endif()

if(OpenGL_FOUND)
    target_link_libraries(HelloTriangle ${OPENGL_LIBRARIES})
else()
    target_link_libraries(HelloTriangle ${OPENGL_LIBRARY})
endif()
