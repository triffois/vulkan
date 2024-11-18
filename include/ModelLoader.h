#pragma once

#include "Model.h"
#include "commonstructs.h"
#include "tiny_gltf.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

class ModelLoader {
  private:
    struct Node {
        glm::vec3 translation{0.0f};
        glm::vec3 scale{1.0f};
        glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
        glm::mat4 transform{1.0f};
        std::vector<Node> children;
        std::vector<uint32_t> meshIndices;
    };

    struct Primitive {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    static void processNode(std::vector<Vertex> &outVertices,
                            std::vector<uint32_t> &outIndices, const Node &node,
                            const tinygltf::Model &model,
                            const glm::mat4 &parentTransform);

    static Node processGLTFNode(const tinygltf::Node &inputNode,
                                const tinygltf::Model &model);

  public:
    static Model loadFromGLTF(const std::string &filename);
};
