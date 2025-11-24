#include <avenir/graphics/Renderer.hpp>
#include <avenir/platform/Window.hpp>
#include <avenir/scene/components/Camera.hpp>
#include <avenir/input/InputManager.hpp>

int main(int argc, char *argv[]) {
    using namespace avenir::graphics;

    avenir::platform::Window window(640, 480, "Simple FPS");
    const auto renderer =
        Renderer::create(window, Renderer::GraphicsAPI::eVulkan);
    avenir::scene::components::Camera mainCamera(
        glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

    auto inputManager = avenir::input::InputManager(window, mainCamera);
    auto lastTime = std::chrono::high_resolution_clock::now();

    while (window.isOpen()) {
        auto now = std::chrono::high_resolution_clock::now();
        const float deltaTime = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        avenir::platform::Window::pollEvents();

        inputManager.update(deltaTime);
        renderer->drawFrame(mainCamera.viewMatrix());
    }

    return 0;
}