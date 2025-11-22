#include <avenir/platform/Window.hpp>
#include <avenir/graphics/Renderer.hpp>

int main(int argc, char *argv[]) {
    using namespace avenir::graphics;

    const avenir::Window window(480, 320, "Simple Renderer");
    const auto renderer = Renderer::create(window.handle(), Renderer::GraphicsAPI::eVulkan);

    while (window.isOpen()) {
        avenir::Window::pollEvents();
        renderer->drawFrame();
    }

    return 0;
}