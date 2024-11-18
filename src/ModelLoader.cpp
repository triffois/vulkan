#include "ModelLoader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// This implementation was *heavily inspired wink wink* by rhusiev's
// https://github.com/triffois/raytracer/blob/main/src/load_model.cpp

void ModelLoader::processNode(std::vector<Vertex> &outVertices,
                              std::vector<uint32_t> &outIndices,
                              const Node &node, const tinygltf::Model &model,
                              const glm::mat4 &parentTransform) {
    glm::mat4 localTransform = parentTransform * node.transform;

    // Process all meshes in this node
    for (uint32_t meshIndex : node.meshIndices) {
        const tinygltf::Mesh &mesh = model.meshes[meshIndex];

        for (const auto &primitive : mesh.primitives) {
            if (primitive.mode != TINYGLTF_MODE_TRIANGLES) {
                continue;
            }

            // Get vertex positions
            const tinygltf::Accessor &posAccessor =
                model.accessors[primitive.attributes.find("POSITION")->second];
            const tinygltf::BufferView &posView =
                model.bufferViews[posAccessor.bufferView];
            const tinygltf::Buffer &posBuffer = model.buffers[posView.buffer];
            const float *positions = reinterpret_cast<const float *>(
                &posBuffer.data[posView.byteOffset + posAccessor.byteOffset]);

            // Get texture coordinates if available
            const float *texCoords = nullptr;
            if (primitive.attributes.find("TEXCOORD_0") !=
                primitive.attributes.end()) {
                const tinygltf::Accessor &texAccessor =
                    model.accessors[primitive.attributes.at("TEXCOORD_0")];
                const tinygltf::BufferView &texView =
                    model.bufferViews[texAccessor.bufferView];
                const tinygltf::Buffer &texBuffer =
                    model.buffers[texView.buffer];
                texCoords = reinterpret_cast<const float *>(
                    &texBuffer
                         .data[texView.byteOffset + texAccessor.byteOffset]);
            }

            // Get indices
            const tinygltf::Accessor &indexAccessor =
                model.accessors[primitive.indices];
            const tinygltf::BufferView &indexView =
                model.bufferViews[indexAccessor.bufferView];
            const tinygltf::Buffer &indexBuffer =
                model.buffers[indexView.buffer];

            uint32_t vertexOffset = static_cast<uint32_t>(outVertices.size());

            // Process vertices
            for (size_t i = 0; i < posAccessor.count; i++) {
                glm::vec4 pos =
                    localTransform * glm::vec4(positions[i * 3],
                                               positions[i * 3 + 1],
                                               positions[i * 3 + 2], 1.0f);

                Vertex vertex{};
                vertex.pos = glm::vec3(pos);
                vertex.color = glm::vec3(1.0f); // Default white color

                if (texCoords) {
                    vertex.texCoord =
                        glm::vec2(texCoords[i * 2], texCoords[i * 2 + 1]);
                }

                outVertices.push_back(vertex);
            }

            // Process indices
            switch (indexAccessor.componentType) {
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                const uint16_t *indices = reinterpret_cast<const uint16_t *>(
                    &indexBuffer.data[indexView.byteOffset +
                                      indexAccessor.byteOffset]);
                for (size_t i = 0; i < indexAccessor.count; i++) {
                    outIndices.push_back(vertexOffset + indices[i]);
                }
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
                const uint32_t *indices = reinterpret_cast<const uint32_t *>(
                    &indexBuffer.data[indexView.byteOffset +
                                      indexAccessor.byteOffset]);
                for (size_t i = 0; i < indexAccessor.count; i++) {
                    outIndices.push_back(vertexOffset + indices[i]);
                }
                break;
            }
            }
        }
    }

    // Process child nodes
    for (const auto &child : node.children) {
        processNode(outVertices, outIndices, child, model, localTransform);
    }
}

ModelLoader::Node ModelLoader::processGLTFNode(const tinygltf::Node &inputNode,
                                               const tinygltf::Model &model) {
    Node node;

    // Get translation
    if (inputNode.translation.size() == 3) {
        node.translation =
            glm::vec3(inputNode.translation[0], inputNode.translation[1],
                      inputNode.translation[2]);
    }

    // Get scale
    if (inputNode.scale.size() == 3) {
        node.scale = glm::vec3(inputNode.scale[0], inputNode.scale[1],
                               inputNode.scale[2]);
    }

    // Get rotation
    if (inputNode.rotation.size() == 4) {
        node.rotation =
            glm::quat(static_cast<float>(inputNode.rotation[3]), // w
                      static_cast<float>(inputNode.rotation[0]), // x
                      static_cast<float>(inputNode.rotation[1]), // y
                      static_cast<float>(inputNode.rotation[2])  // z
            );
    }

    // Calculate transform
    if (inputNode.matrix.size() == 16) {
        node.transform = glm::mat4(glm::make_mat4x4(inputNode.matrix.data()));
    } else {
        glm::mat4 translation =
            glm::translate(glm::mat4(1.0f), node.translation);
        glm::mat4 rotation = glm::mat4_cast(node.rotation);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), node.scale);
        node.transform = translation * rotation * scale;
    }

    // Store mesh index if this node has a mesh
    if (inputNode.mesh >= 0) {
        node.meshIndices.push_back(inputNode.mesh);
    }

    // Process children
    for (int childIdx : inputNode.children) {
        node.children.push_back(processGLTFNode(model.nodes[childIdx], model));
    }

    return node;
}

Model ModelLoader::loadFromGLTF(const std::string &filename) {
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool isBinary = filename.length() > 4 &&
                    filename.substr(filename.length() - 4) == ".glb";

    bool loaded =
        isBinary ? loader.LoadBinaryFromFile(&gltfModel, &err, &warn, filename)
                 : loader.LoadASCIIFromFile(&gltfModel, &err, &warn, filename);

    if (!warn.empty()) {
        std::cerr << "GLTF Warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        throw std::runtime_error("GLTF Error: " + err);
    }

    if (!loaded) {
        throw std::runtime_error("Failed to load GLTF file: " + filename);
    }

    // Process the scene
    const tinygltf::Scene &scene =
        gltfModel
            .scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Process each root node
    for (int nodeIdx : scene.nodes) {
        Node rootNode = processGLTFNode(gltfModel.nodes[nodeIdx], gltfModel);
        processNode(vertices, indices, rootNode, gltfModel, glm::mat4(1.0f));
    }

    std::vector<uint16_t> indices16;
    indices16.reserve(indices.size());
    for (uint32_t idx : indices) {
        indices16.push_back(static_cast<uint16_t>(idx));
    }

    return Model(vertices, indices16);
}
