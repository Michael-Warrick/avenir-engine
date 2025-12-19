#include <avenir/graphics/Renderer.hpp>
#include <avenir/platform/Window.hpp>
#include <avenir/scene/components/Camera.hpp>

int main(int argc, char *argv[]) {
    avenir::platform::Window window(480, 320, "Simple Renderer");
    const auto renderer = avenir::graphics::Renderer::create(
        window, avenir::graphics::Api::eVulkan);

    const avenir::scene::components::Camera camera(
        glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

    while (window.isOpen()) {
        avenir::platform::Window::pollEvents();
        renderer->drawFrame(camera.viewMatrix());
    }

    return 0;
}