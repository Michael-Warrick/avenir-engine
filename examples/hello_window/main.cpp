#include <iostream>

#include <avenir/platform/Window.hpp>
#include <avenir/graphics/Renderer.hpp>

int main(int argc, const char *argv[]) {
    const avenir::platform::Window window(480, 320, "Hello Window!");

    while (window.isOpen()) {
        avenir::platform::Window::pollEvents();
    }

    return 0;
}