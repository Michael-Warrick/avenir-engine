#include "avenir/graphics/Mesh.hpp"

namespace avenir::graphics {
std::vector<Vertex> Mesh::vertices() const { return m_vertices; }

std::vector<uint16_t> Mesh::indices() const { return m_indices; }

void Mesh::setVertices(const std::vector<Vertex> &vertices) {
    m_vertices = vertices;
}

void Mesh::setIndices(const std::vector<uint16_t> &indices) {
    m_indices = indices;
}

}  // namespace avenir::graphics