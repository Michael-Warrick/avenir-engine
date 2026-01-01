#ifndef AVENIR_SCENE_COMPONENT_HPP
#define AVENIR_SCENE_COMPONENT_HPP

#include <string>

struct Component {
    virtual ~Component() = default;

    [[nodiscard]] virtual std::string name() const = 0;
};

#endif  // AVENIR_SCENE_COMPONENT_HPP
