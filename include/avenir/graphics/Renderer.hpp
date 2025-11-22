#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <GLFW/glfw3.h>

#include <memory>

namespace avenir::graphics {
class Renderer {
public:
    enum class GraphicsAPI { eVulkan = 0 };

    static std::unique_ptr<Renderer> create(GLFWwindow *window,
                                            GraphicsAPI api);
    virtual ~Renderer() = default;

    virtual void drawFrame() = 0;
    virtual void onFramebufferResize(int width, int height) = 0;

    static void framebufferResizeCallback(GLFWwindow *window, int width,
                                          int height);
};
}  // namespace avenir::graphics

#endif  // RENDERER_HPP