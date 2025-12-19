#include <avenir/graphics/Renderer.hpp>
#include <avenir/input/InputManager.hpp>
#include <avenir/platform/Time.hpp>
#include <avenir/platform/Window.hpp>
#include <avenir/scene/components/Camera.hpp>

#include "FPSController.hpp"

int main(int argc, char *argv[]) {
    auto window = avenir::platform::Window(640, 480, "Simple FPS");

    const auto renderer = avenir::graphics::Renderer::create(
        window, avenir::graphics::Api::eVulkan);

    avenir::scene::components::Camera mainCamera(
        glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

    avenir::input::InputManager inputManager(window);

    const FPSController fpsController(inputManager, mainCamera);

    avenir::platform::Time time;
    while (window.isOpen()) {
        time.tick();
        avenir::platform::Window::pollEvents();

        fpsController.update(time.deltaTime());

        renderer->drawFrame(mainCamera.viewMatrix());
    }

    return 0;
}
