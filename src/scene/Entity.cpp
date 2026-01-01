#include "avenir/scene/Entity.hpp"

#include <iostream>

namespace avenir::scene {

Entity::Entity(const uint32_t id) { m_id = id; }

void Entity::listComponents() const {
    for (const auto &component : m_components) {
        std::cout << "Component: " << component->name() << "\n";
    }
}

uint32_t Entity::id() const { return m_id; }

const std::vector<std::unique_ptr<Component>> &Entity::components() const {
    return m_components;
}

}  // namespace avenir::scene