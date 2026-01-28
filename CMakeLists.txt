cmake_minimum_required(VERSION 3.20)
project(avenir VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

add_library(${PROJECT_NAME}
        # Platform
        src/platform/Window.cpp
        src/platform/Time.cpp

        # Input-Output
        src/input/InputManager.cpp

        # Scene
        src/scene/Scene.cpp
        src/scene/Entity.cpp
        src/scene/components/Transform.cpp
        src/scene/components/MeshRenderer.cpp
        src/scene/components/Camera.cpp

        # Graphics (API-agnostic)
        src/graphics/Renderer.cpp
        src/graphics/Mesh.cpp
        src/graphics/stb_image_impl.cpp

        # Vulkan
        src/graphics/vulkan/VulkanRenderer.cpp
        src/graphics/vulkan/VulkanInstance.cpp
        src/graphics/vulkan/VulkanMesh.cpp

        # Debugging/Profiling
        src/debug/Debug.cpp
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:>/include
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
        glfw
        glm
        Vulkan::Vulkan
)

target_link_libraries(${PROJECT_NAME}
        glfw
        glm
        Vulkan::Vulkan
)

add_subdirectory(examples)