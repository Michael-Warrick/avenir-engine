#include <avenir/avenir.hpp>

int main(int argc, char *argv[]) {
    avenir::Window window(480, 320, "Simple Renderer");
    const auto renderer =
        avenir::Renderer::create(window, avenir::GraphicsApi::eVulkan);

    avenir::Scene scene;

    avenir::Entity &camera = scene.createEntity();
    camera.component<avenir::Transform>().position =
        glm::vec3(0.0f, 0.0f, 2.0f);
    camera.addComponent<avenir::Camera>();

    while (window.isOpen()) {
        avenir::platform::Window::pollEvents();
        renderer->drawFrame(scene.entityInverseWorldMatrix(camera.id()));
    }

    return 0;
}