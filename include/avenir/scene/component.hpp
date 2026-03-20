#ifndef AVENIR_SCENE_COMPONENT_HPP
#define AVENIR_SCENE_COMPONENT_HPP

#include <string>
#include <memory>

struct Component {
    virtual ~Component() = default;

    [[nodiscard]] virtual std::unique_ptr<Component> clone() const = 0;

    [[nodiscard]] virtual std::string name() const = 0;
};

#endif  // AVENIR_SCENE_COMPONENT_HPP
