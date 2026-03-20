#ifndef AVENIR_AVENIR_HPP
#define AVENIR_AVENIR_HPP

#include "avenir/platform/time.hpp"
#include "avenir/platform/window.hpp"
#include "avenir/input/input_manager.hpp"
#include "avenir/graphics/renderer.hpp"
#include "avenir/scene/entity.hpp"
#include "avenir/scene/components/camera.hpp"
#include "avenir/scene/components/mesh_renderer.hpp"
#include "avenir/scene/components/transform.hpp"
#include "avenir/scene/scene.hpp"
#include "avenir/debug/debug.hpp"
#include "avenir/graphics/mesh.hpp"

namespace avenir {

using Window = platform::Window;
using Time = platform::Time;

using InputManager = input::InputManager;
using Key = input::Key;
using CursorMode = input::CursorMode;

using Renderer = graphics::Renderer;
using GraphicsApi = graphics::Api;

using Scene = scene::Scene;
using Entity = scene::Entity;

using Transform = scene::components::Transform;
using Camera = scene::components::Camera;
using MeshRenderer = scene::components::MeshRenderer;

}  // namespace avenir

#endif  // AVENIR_AVENIR_HPP