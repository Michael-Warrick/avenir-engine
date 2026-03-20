cmake_minimum_required(VERSION 3.20)
project(avenir VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glm)

add_library(${PROJECT_NAME}
        # Platform
        src/platform/window.cpp
        src/platform/time.cpp

        # Input-Output
        src/input/input_manager.cpp

        # Scene
        src/scene/scene.cpp
        src/scene/entity.cpp
        src/scene/components/transform.cpp
        src/scene/components/mesh_renderer.cpp
        src/scene/components/camera.cpp

        # Graphics (API-agnostic)
        src/graphics/renderer.cpp
        src/graphics/mesh.cpp
        src/graphics/stb_image_impl.cpp

        # Vulkan
        src/graphics/vulkan/vulkan_renderer.cpp
        src/graphics/vulkan/vulkan_instance.cpp
        src/graphics/vulkan/vulkan_surface.cpp
        src/graphics/vulkan/vulkan_physical_device.cpp
        src/graphics/vulkan/vulkan_device.cpp
        src/graphics/vulkan/vulkan_swapchain.cpp
        src/graphics/vulkan/vulkan_mesh.cpp

        # Debugging/Profiling
        src/debug/debug.cpp
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