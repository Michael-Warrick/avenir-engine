#include "avenir/scene/Entity.hpp"

#include "avenir/scene/components/Camera.hpp"
#include "avenir/scene/components/Transform.hpp"

#include <iostream>
#include <algorithm>

namespace avenir::scene {

Entity::Entity(const uint32_t id) {
    m_id = id;
    m_components.emplace_back(
        std::make_unique<avenir::scene::components::Transform>());
}

Entity::Entity(const Entity &other) {
    m_id = other.id();
    m_parent = other.parent();
    m_children = other.children();

    m_components.clear();
    m_components.reserve(other.components().size());
    for (const auto &pComponent : other.components()) {
        m_components.emplace_back(pComponent->clone());
    }
}

Entity &Entity::operator=(Entity const &other) {
    m_id = other.id();
    m_parent = other.parent();
    m_children = other.children();

    m_components.clear();
    m_components.reserve(other.components().size());
    for (const auto &pComponent : other.components()) {
        m_components.emplace_back(pComponent->clone());
    }

    return *this;
}

void Entity::listComponents() const {
    std::cout << "[Entity] id: " << m_id << ", Components:\n";
    for (const auto &component : m_components) {
        std::cout << "\t\t " << component->name() << "\n";
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