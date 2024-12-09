#include "ModelLoader.h"
#include "TextureManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// This implementation was *heavily inspired wink wink* by rhusiev's
// https://github.com/triffois/raytracer/blob/main/src/load_model.cpp

void ModelLoader::processNode(
    std::map<int32_t, ProcessedPrimitive> &materialPrimitives, const Node &node,
    const tinygltf::Model &model, const glm::mat4 &parentTransform) {
    glm::mat4 localTransform = parentTransform * node.transform;

    // Process all meshes in this node
    for (uint32_t meshIndex : node.meshIndices) {
        const tinygltf::Mesh &mesh = model.meshes[meshIndex];

        for (const auto &primitive : mesh.primitives) {
            if (primitive.mode != TINYGLTF_MODE_TRIANGLES) {
                continue;
            }

            ProcessedPrimitive processedPrim =
                processPrimitive(primitive, model, localTransform);

            // Merge with existing primitive of the same material or create new
            auto &materialPrim =
                materialPrimitives[processedPrim.materialIndex];
            materialPrim.materialIndex = processedPrim.materialIndex;

            // Adjust indices for the merged vertex buffer
            uint32_t vertexOffset =
                static_cast<uint32_t>(materialPrim.vertices.size());
            for (uint32_t index : processedPrim.indices) {
                materialPrim.indices.push_back(vertexOffset + index);
            }

            // Append vertices
            materialPrim.vertices.insert(materialPrim.vertices.end(),
                                         processedPrim.vertices.begin(),
                                         processedPrim.vertices.end());
        }
    }

    // Process child nodes
    for (const auto &child : node.children) {
        processNode(materialPrimitives, child, model, localTransform);
    }
}

ModelLoader::ProcessedPrimitive
ModelLoader::processPrimitive(const tinygltf::Primitive &primitive,
                              const tinygltf::Model &model,
                              const glm::mat4 &transform) {
    ProcessedPrimitive result;
    result.materialIndex = primitive.material;

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
    if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
        const tinygltf::Accessor &texAccessor =
            model.accessors[primitive.attributes.at("TEXCOORD_0")];
        const tinygltf::BufferView &texView =
            model.bufferViews[texAccessor.bufferView];
        const tinygltf::Buffer &texBuffer = model.buffers[texView.buffer];
        texCoords = reinterpret_cast<const float *>(
            &texBuffer.data[texView.byteOffset + texAccessor.byteOffset]);
    }

    // Process vertices
    for (size_t i = 0; i < posAccessor.count; i++) {
        glm::vec4 pos =
            transform * glm::vec4(positions[i * 3], positions[i * 3 + 1],
                                  positions[i * 3 + 2], 1.0f);

        Vertex vertex{};
        vertex.pos = glm::vec3(pos);
        vertex.color = glm::vec3(1.0f); // Default white color

        if (texCoords) {
            vertex.texCoord = glm::vec2(texCoords[i * 2], texCoords[i * 2 + 1]);
        }

        result.vertices.push_back(vertex);
    }

    // Process indices
    const tinygltf::Accessor &indexAccessor =
        model.accessors[primitive.indices];
    const tinygltf::BufferView &indexView =
        model.bufferViews[indexAccessor.bufferView];
    const tinygltf::Buffer &indexBuffer = model.buffers[indexView.buffer];

    switch (indexAccessor.componentType) {
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
        const uint16_t *indices = reinterpret_cast<const uint16_t *>(
            &indexBuffer.data[indexView.byteOffset + indexAccessor.byteOffset]);
        for (size_t i = 0; i < indexAccessor.count; i++) {
            result.indices.push_back(indices[i]);
        }
        break;
    }
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
        const uint32_t *indices = reinterpret_cast<const uint32_t *>(
            &indexBuffer.data[indexView.byteOffset + indexAccessor.byteOffset]);
        for (size_t i = 0; i < indexAccessor.count; i++) {
            result.indices.push_back(indices[i]);
        }
        break;
    }
    }

    return result;
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

TextureID ModelLoader::processImage(const tinygltf::Image &image,
                                    const tinygltf::Model &model,
                                    TextureManager &textureManager) {
    TextureData textureData;
    textureData.width = image.width;
    textureData.height = image.height;
    textureData.channels = image.component;
    textureData.mimeType = image.mimeType;

    if (image.image.size() > 0) {
        // Image data is already loaded in memory
        textureData.pixels = image.image;
    } else if (image.bufferView >= 0) {
        // Image data is stored in a buffer view
        const tinygltf::BufferView &bufferView =
            model.bufferViews[image.bufferView];
        const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];

        const unsigned char *data = buffer.data.data() + bufferView.byteOffset;
        size_t dataSize = bufferView.byteLength;

        textureData.pixels.resize(dataSize);
        std::memcpy(textureData.pixels.data(), data, dataSize);
    }

    // Register the texture with the TextureManager
    return textureManager.registerTexture(textureData);
}

MaterialInstance
ModelLoader::processMaterial(const tinygltf::Material &material,
                             const tinygltf::Model &model,
                             TextureManager &textureManager) {
    MaterialInstance materialInstance;

    // Process base color texture (index 0)
    if (material.pbrMetallicRoughness.baseColorTexture.index >= 0) {
        int textureIndex = material.pbrMetallicRoughness.baseColorTexture.index;
        int imageIndex = model.textures[textureIndex].source;
        if (imageIndex >= 0) {
            materialInstance.textureIds[0] =
                processImage(model.images[imageIndex], model, textureManager);
        }
    }

    // Process metallic-roughness texture (index 1)
    if (material.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0) {
        int textureIndex =
            material.pbrMetallicRoughness.metallicRoughnessTexture.index;
        int imageIndex = model.textures[textureIndex].source;
        if (imageIndex >= 0) {
            materialInstance.textureIds[1] =
                processImage(model.images[imageIndex], model, textureManager);
        }
    }

    // Process normal map (index 2)
    if (material.normalTexture.index >= 0) {
        int imageIndex = model.textures[material.normalTexture.index].source;
        if (imageIndex >= 0) {
            materialInstance.textureIds[2] =
                processImage(model.images[imageIndex], model, textureManager);
        }
    }

    // Process occlusion texture (index 3)
    if (material.occlusionTexture.index >= 0) {
        int imageIndex = model.textures[material.occlusionTexture.index].source;
        if (imageIndex >= 0) {
            materialInstance.textureIds[3] =
                processImage(model.images[imageIndex], model, textureManager);
        }
    }

    return materialInstance;
}

std::vector<TextureID>
ModelLoader::processTextures(const tinygltf::Model &model,
                             TextureManager &textureManager) {
    std::vector<TextureID> textures;
    textures.reserve(model.images.size());

    for (const auto &image : model.images) {
        textures.push_back(processImage(image, model, textureManager));
    }

    return textures;
}

Model ModelLoader::loadFromGLTF(const std::string &filename,
                                GlobalResources &resources,
                                TextureManager &textureManager) {
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

    Model model;
    std::map<int32_t, ProcessedPrimitive> materialPrimitives;

    // Process each root node
    const tinygltf::Scene &gltfScene =
        gltfModel
            .scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];

    for (int nodeIdx : gltfScene.nodes) {
        Node rootNode = processGLTFNode(gltfModel.nodes[nodeIdx], gltfModel);
        processNode(materialPrimitives, rootNode, gltfModel, glm::mat4(1.0f));
    }

    // Create render batches from processed primitives
    createRenderBatches(materialPrimitives, gltfModel, resources, model,
                        textureManager);

    return model;
}

void ModelLoader::createRenderBatches(
    const std::map<int32_t, ProcessedPrimitive> &primitives,
    const tinygltf::Model &source, GlobalResources &resources,
    Model &destination, TextureManager &textureManager) {

    auto &meshManager = resources.getMeshManager();

    for (const auto &[materialIndex, primitive] : primitives) {
        // Register mesh with MeshManager
        MeshID meshId =
            meshManager.registerMesh(primitive.vertices, primitive.indices);

        // Create a single instance for this primitive
        Instance instance;
        if (materialIndex >= 0) {
            instance.material = processMaterial(source.materials[materialIndex],
                                                source, textureManager);
        }

        // Create render batch
        RenderBatch batch;
        batch.meshId = meshId;
        batch.instances.push_back(instance);

        destination.addBatch(std::move(batch));
    }
}
