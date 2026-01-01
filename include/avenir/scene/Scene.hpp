#ifndef AVENIR_SCENE_SCENE_HPP
#define AVENIR_SCENE_SCENE_HPP

#include <unordered_map>

#include "avenir/scene/Entity.hpp"

namespace avenir::scene {

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    Entity &createEntity();
    std::optional<Entity *> findEntityById(uint32_t id);

    void setEntityParent(uint32_t child, std::optional<uint32_t> parent);
    void detatchEntityFromParent(uint32_t child);

    void printEntityIds();

private:
    std::unordered_map<uint32_t, Entity> m_entities;
};

}  // namespace avenir::scene

#endif  // AVENIR_SCENE_SCENE_HPP
