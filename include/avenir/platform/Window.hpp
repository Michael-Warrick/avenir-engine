#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>

#include <string>

#include "avenir/graphics/Renderer.hpp"
#include "avenir/input/InputManager.hpp"

namespace avenir {
// Forward declarations
namespace graphics { class Renderer; }
namespace input { class InputManager; }
} // namespace avenir

namespace avenir::platform {
class Window {
public:
    struct Context {
        // Necessary to share the GLFW window user pointer
        graphics::Renderer *renderer = nullptr;
        input::InputManager *inputManager = nullptr;
    };

    Window() = default;
    Window(uint32_t width, uint32_t height, const std::string &title);
    ~Window();

    [[nodiscard]] bool isOpen() const;
    static void pollEvents();
    [[nodiscard]] uint32_t getWidth() const;
    [[nodiscard]] uint32_t getHeight() const;

    [[nodiscard]] GLFWwindow *handle() const;
    Context &context();

private:
    GLFWwindow *m_glfwWindow = nullptr;
    Context m_context{};
};
}  // namespace avenir::platform

#endif // WINDOW_HPP