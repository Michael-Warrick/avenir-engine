#include <iostream>

#include <avenir/platform/window.hpp>
#include <avenir/graphics/renderer.hpp>

int main(int argc, const char *argv[]) {
    const avenir::platform::Window window(480, 320, "Hello Window!");

    while (window.isOpen()) {
        avenir::platform::Window::pollEvents();
    }

    return 0;
}