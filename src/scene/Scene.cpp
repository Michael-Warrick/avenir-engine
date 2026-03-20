#include "avenir/scene/Scene.hpp"

#include "avenir/avenir.hpp"

#include <iostream>
#include <ranges>

namespace avenir::scene {

Entity &Scene::createEntity() {
    static uint32_t id = 0;
    auto [it, _] = m_entities.try_emplace(id, Entity(id));

    id++;

    return it->second;
}

std::optional<Entity *> Scene::findEntityById(const uint32_t id) {
    const auto it = m_entities.find(id);

    return it != m_entities.end() ? &it->second : nullptr;
}

void Scene::setEntityParent(const uint32_t childId,
                            const std::optional<uint32_t> parent) {
    const std::optional<Entity *> entity = findEntityById(childId);
    if (parent && (*parent == childId)) {
        throw std::runtime_error("Error: Entity cannot parent itself!\n");
    }

    // Remove child entity from previous parent
    if (entity.value()->parent()) {
        const std::optional<Entity *> previousParent =
            findEntityById(*entity.value()->parent());
        previousParent.value()->removeChild(childId);
    }

    entity.value()->setParent(parent);

    // Assign the child entity to the new parent
    if (parent) {
        const std::optional<Entity *> newParent = findEntityById(*parent);
        newParent.value()->addChild(childId);
    }
}

void Scene::setEntityParentToNone(const uint32_t childId) {
    setEntityParent(childId, std::nullopt);
}

glm::mat4 Scene::entityWorldMatrix(const uint32_t id) {
    const std::optional<Entity *> entity = findEntityById(id);
    const auto &entityTransform =
        entity.value()->component<avenir::scene::components::Transform>();

    const glm::mat4 localMatrix = entityTransform.localMatrix();
    if (!entity.value()->parent().has_value()) {
        return localMatrix;
    }

    return entityWorldMatrix(*entity.value()->parent()) * localMatrix;
}

glm::mat4 Scene::entityInverseWorldMatrix(const uint32_t id) {
    return glm::inverse(entityWorldMatrix(id));
}

void Scene::printEntityIds() {
    for (const auto &key : m_entities | std::views::keys) {
        std::cout << "[Scene] Entity ID: " << key << "\n";
    }
}

}  // namespace avenir::scene
