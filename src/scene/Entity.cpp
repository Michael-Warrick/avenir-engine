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

std::optional<uint32_t> Entity::parent() const { return m_parent; }

const std::vector<uint32_t> &Entity::children() const { return m_children; }

void Entity::setParent(const std::optional<uint32_t> id) { m_parent = id; }

void Entity::addChild(uint32_t id) {
    if (std::ranges::find(m_children, id) == m_children.end()) {
        m_children.emplace_back(id);
    }
}

void Entity::removeChild(const uint32_t id) {
    m_children.erase(std::ranges::remove(m_children, id).begin(),
                     m_children.end());
}

}  // namespace avenir::scene