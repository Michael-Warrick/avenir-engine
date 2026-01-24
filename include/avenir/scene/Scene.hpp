#ifndef AVENIR_SCENE_SCENE_HPP
#define AVENIR_SCENE_SCENE_HPP

#include <unordered_map>

#include <glm/mat4x4.hpp>

#include "avenir/scene/Entity.hpp"

namespace avenir::scene {

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    Entity &createEntity();
    std::optional<Entity *> findEntityById(uint32_t id);

    void setEntityParent(uint32_t child, std::optional<uint32_t> parent);
    void detachEntityFromParent(uint32_t child);

    glm::mat4 entityWorldMatrix(uint32_t id);
    glm::mat4 entityInverseWorldMatrix(uint32_t id);

    void printEntityIds();

private:
    std::unordered_map<uint32_t, Entity> m_entities;
};

}  // namespace avenir::scene

#endif  // AVENIR_SCENE_SCENE_HPP
