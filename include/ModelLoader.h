#pragma once

#include "GlobalResources.h"
#include "Model.h"
#include "TextureManager.h"
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

    struct ProcessedPrimitive {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;
        int32_t materialIndex = -1;
    };

    static void
    processNode(std::map<int32_t, ProcessedPrimitive> &materialPrimitives,
                const Node &node, const tinygltf::Model &model,
                const glm::mat4 &parentTransform);

    static ProcessedPrimitive
    processPrimitive(const tinygltf::Primitive &primitive,
                     const tinygltf::Model &model, const glm::mat4 &transform);

    static Node processGLTFNode(const tinygltf::Node &inputNode,
                                const tinygltf::Model &model);

    static TextureID processImage(const tinygltf::Image &image,
                                  const tinygltf::Model &model,
                                  TextureManager &textureManager);
    std::vector<TextureID> processTextures(const tinygltf::Model &model,
                                           TextureManager &textureManager);
    static MaterialInstance processMaterial(const tinygltf::Material &material,
                                            const tinygltf::Model &model,
                                            TextureManager &textureManager);

    static void
    createRenderBatches(const std::map<int32_t, ProcessedPrimitive> &primitives,
                        const tinygltf::Model &source,
                        GlobalResources &resources, Model &destination,
                        TextureManager &textureManager);

  public:
    static Model loadFromGLTF(const std::string &filename,
                              GlobalResources &resources,
                              TextureManager &textureManager);
};
