#ifndef AVENIR_MESH_HPP
#define AVENIR_MESH_HPP

#include <glm/glm.hpp>

namespace avenir::graphics {
struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 textureCoordinates;
};

class Mesh {
public:
    virtual ~Mesh() = default;

protected:
    [[nodiscard]] std::vector<Vertex> vertices() const;
    [[nodiscard]] std::vector<uint16_t> indices() const;

    void setVertices(const std::vector<Vertex> &vertices);
    void setIndices(const std::vector<uint16_t> &indices);

    std::vector<Vertex> m_vertices;
    std::vector<uint16_t> m_indices;
};

}  // namespace avenir::graphics

#endif  // AVENIR_MESH_HPP
