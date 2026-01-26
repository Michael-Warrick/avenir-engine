#ifndef AVENIR_AVENIR_HPP
#define AVENIR_AVENIR_HPP

#include "avenir/platform/Time.hpp"
#include "avenir/platform/Window.hpp"
#include "avenir/input/InputManager.hpp"
#include "avenir/graphics/Renderer.hpp"
#include "avenir/scene/Entity.hpp"
#include "avenir/scene/components/Camera.hpp"
#include "avenir/scene/components/MeshRenderer.hpp"
#include "avenir/scene/components/Transform.hpp"
#include "avenir/scene/Scene.hpp"
#include "avenir/debug/Debug.hpp"

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