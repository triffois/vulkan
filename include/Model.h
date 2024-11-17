#pragma once

#include "commonstructs.h"
#include <vector>

class Model {
  public:
    Model(const std::vector<Vertex> &vertices,
          const std::vector<uint16_t> &indices)
        : vertices(vertices), indices(indices) {}

    const std::vector<Vertex> &getVertices() const { return vertices; }
    const std::vector<uint16_t> &getIndices() const { return indices; }

  private:
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
};
