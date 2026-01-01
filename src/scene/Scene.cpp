#include "avenir/scene/Scene.hpp"

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

void Scene::printEntityIds() {
    for (const auto &key : m_entities | std::views::keys) {
        std::cout << "[Scene] Entity ID: " << key << "\n";
    }
}

}  // namespace avenir::scene
