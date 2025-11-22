#include <iostream>

#include <avenir/platform/Window.hpp>
#include <avenir/graphics/Renderer.hpp>

int main(int argc, const char *argv[]) {
    const avenir::Window window(480, 320, "Hello Window!");

    while (window.isOpen()) {
        avenir::Window::pollEvents();
    }

    return 0;
}