#ifndef AVENIR_SCENE_ENTITY_HPP
#define AVENIR_SCENE_ENTITY_HPP

#include <memory>
#include <vector>

#include "avenir/scene/Component.hpp"

namespace avenir::scene {

class Entity {
public:
    explicit Entity(uint32_t id);

    template <typename T, typename... Args>
    T &addComponent(Args &&...args) {
        static_assert(std::is_base_of_v<Component, T>);
        auto pointer = std::make_unique<T>(std::forward<Args>(args)...);
        T &reference = *pointer;
        m_components.emplace_back(std::move(pointer));

        return reference;
    }

    template <typename T>
    T &component() {
        static_assert(std::is_base_of_v<Component, T>,
                      "T must derive from Component");

        for (auto &component : m_components) {
            if (auto *casted = dynamic_cast<T *>(component.get())) {
                return *casted;
            }
        }

        throw std::runtime_error("Entity does not have requested component!\n");
    }

    template <typename T>
    const T &component() const {
        static_assert(std::is_base_of_v<Component, T>,
                      "T must derive from Component");

        for (const auto &component : m_components) {
            if (auto *casted = dynamic_cast<const T *>(component.get())) {
                return *casted;
            }
        }

        throw std::runtime_error("Entity does not have requested component!\n");
    }

    void listComponents() const;

    [[nodiscard]] uint32_t id() const;
    [[nodiscard]] std::optional<uint32_t> parent() const;
    [[nodiscard]] const std::vector<uint32_t> &children() const;
    [[nodiscard]] const std::vector<std::unique_ptr<Component>> &components()
        const;

    void setParent(std::optional<uint32_t> id);
    void addChild(uint32_t id);
    void removeChild(uint32_t id);

private:
    uint32_t m_id;
    std::optional<uint32_t> m_parent;
    std::vector<uint32_t> m_children;
    std::vector<std::unique_ptr<Component>> m_components;
};

}  // namespace avenir::scene

#endif  // AVENIR_SCENE_ENTITY_HPP