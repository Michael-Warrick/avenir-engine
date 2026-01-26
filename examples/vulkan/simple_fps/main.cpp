#include <avenir/avenir.hpp>

#include "FPSController.hpp"

int main(int argc, char *argv[]) {
    avenir::Window window(960, 720, "Simple FPS");
    avenir::InputManager inputManager(window);
    avenir::Time time;

    const auto renderer =
        avenir::Renderer::create(window, avenir::GraphicsApi::eVulkan);

    avenir::Scene scene;

    avenir::Entity &player = scene.createEntity();
    player.component<avenir::Transform>().position =
        glm::vec3(0.0f, 0.0f, 2.0f);

    avenir::Entity &camera = scene.createEntity();
    camera.component<avenir::Transform>().position =
        glm::vec3(0.0f, 0.8f, 0.0f);
    camera.addComponent<avenir::Camera>();

    scene.setEntityParent(camera.id(), player.id());

    FPSController fpsController(player, scene, inputManager);
    while (window.isOpen()) {
        time.tick();
        avenir::Window::pollEvents();

        fpsController.update(time.deltaTime());

        renderer->drawFrame(scene.entityInverseWorldMatrix(camera.id()));
    }

    return 0;
}
