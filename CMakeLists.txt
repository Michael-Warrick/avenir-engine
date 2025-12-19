cmake_minimum_required(VERSION 3.20)
project(avenir VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

add_library(${PROJECT_NAME}
        src/platform/Window.cpp
        src/platform/Time.cpp
        src/input/InputManager.cpp

        src/scene/Scene.cpp
        src/scene/components/Camera.cpp

        src/graphics/Renderer.cpp
        src/graphics/vulkan/VulkanRenderer.cpp
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