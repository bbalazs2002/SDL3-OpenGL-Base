cmake_minimum_required(VERSION 3.20)
project(SDL3-OpenGL-Base)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find all .cpp, .hpp and .h files in the directory
file(GLOB_RECURSE SOURCES
    "src/*.cpp" 
    "src/*.h"
    "src/*.hpp"
)

# 2. Look for packages (using the correct Case-Sensitive names for vcpkg)
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(GLEW REQUIRED) # Use uppercase GLEW
find_package(glm REQUIRED)
find_package(imgui CONFIG REQUIRED) # ImGui often needs the CONFIG flag

# 3. Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Enable experimental GLM extensions globally
target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_ENABLE_EXPERIMENTAL)

# Define the project root directory path for the source code


# 4. Link libraries (using the specific vcpkg targets)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    SDL3::SDL3 
    SDL3_image::SDL3_image
    GLEW::GLEW        # Most likely target name in vcpkg
    glm::glm
    imgui::imgui
)