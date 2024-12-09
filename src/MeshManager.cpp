#include "MeshManager.h"
#include "Buffer.h"
#include "Device.h"
#include <cstring>

void MeshManager::init(Device *device) { this->device = device; }

void MeshManager::cleanup() { meshes.clear(); }

MeshID MeshManager::registerMesh(const std::vector<Vertex> &vertices,
                                 const std::vector<uint16_t> &indices) {
    auto mesh = std::make_unique<Mesh>();

    // Create vertex buffer
    VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();

    // Create staging buffer for vertices
    Buffer stagingBuffer(
        device, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    // Copy vertex data to staging buffer
    void *data;
    stagingBuffer.map(&data);
    memcpy(data, vertices.data(), vertexBufferSize);
    stagingBuffer.unmap();

    // Create device local vertex buffer
    mesh->vertexBuffer = std::make_unique<Buffer>(
        device, vertexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

    // Copy from staging buffer to device local buffer
    mesh->vertexBuffer->copyFrom(stagingBuffer, vertexBufferSize);

    // Create index buffer
    VkDeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();

    // Create staging buffer for indices
    Buffer indexStagingBuffer(
        device, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    // Copy index data to staging buffer
    indexStagingBuffer.map(&data);
    memcpy(data, indices.data(), indexBufferSize);
    indexStagingBuffer.unmap();

    // Create device local index buffer
    mesh->indexBuffer = std::make_unique<Buffer>(
        device, indexBufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

    // Copy from staging buffer to device local buffer
    mesh->indexBuffer->copyFrom(indexStagingBuffer, indexBufferSize);

    mesh->indexCount = static_cast<uint32_t>(indices.size());

    // Store mesh and return ID
    MeshID id = nextMeshId++;
    meshes[id] = std::move(mesh);
    return id;
}

const Mesh *MeshManager::getMesh(MeshID id) const {
    auto it = meshes.find(id);
    return it != meshes.end() ? it->second.get() : nullptr;
}
