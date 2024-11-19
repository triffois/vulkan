#pragma once

#include "commonstructs.h"
#include <vector>
#include <string>

struct TextureData {
    std::vector<unsigned char> pixels;
    int width;
    int height;
    int channels;
    std::string mimeType;
};

class Model {
  public:
    Model(const std::vector<Vertex> &vertices,
          const std::vector<uint16_t> &indices,
          const std::vector<TextureData> &textures = {})
        : vertices(vertices), indices(indices), textures(textures) {}

    const std::vector<Vertex> &getVertices() const { return vertices; }
    const std::vector<uint16_t> &getIndices() const { return indices; }
    const std::vector<TextureData> &getTextures() const { return textures; }

  private:
    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;
    std::vector<TextureData> textures;
};
