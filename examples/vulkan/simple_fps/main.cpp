#include <avenir/avenir.hpp>

#include "FPSController.hpp"

int main(int argc, char *argv[]) {
    auto window = avenir::platform::Window(960, 720, "Simple FPS");
    avenir::InputManager inputManager(window);
    avenir::Time time;

    const auto renderer =
        avenir::Renderer::create(window, avenir::GraphicsApi::eVulkan);

    avenir::Scene scene;

    avenir::Entity &camera = scene.createEntity();
    camera.addComponent<avenir::Transform>(glm::vec3(0.0f, 0.0f, 2.0f));
    camera.addComponent<avenir::Camera>();

    avenir::Entity &player = scene.createEntity();
    player.addComponent<avenir::Transform>();

    avenir::Entity &enemy = scene.createEntity();
    enemy.addComponent<avenir::Transform>();

    scene.printEntityIds();

    FPSController fpsController(inputManager, camera);

    while (window.isOpen()) {
        time.tick();
        avenir::Window::pollEvents();

        fpsController.update(time.deltaTime());

        renderer->drawFrame(
            camera.component<avenir::Transform>().inverseWorldMatrix());
    }

    return 0;
}
