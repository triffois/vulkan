#pragma once

#include "Buffer.h"
#include "Device.h"
#include "commonstructs.h"
#include <memory>
#include <unordered_map>
#include <vector>

using MeshID = uint32_t;

struct Mesh {
    std::unique_ptr<Buffer> vertexBuffer;
    std::unique_ptr<Buffer> indexBuffer;
    uint32_t indexCount;
};

class MeshManager {
  public:
    MeshManager() = default;
    ~MeshManager() = default;

    MeshManager(const MeshManager &) = delete;
    MeshManager &operator=(const MeshManager &) = delete;

    void init(Device *device);
    void cleanup();

    MeshID registerMesh(const std::vector<Vertex> &vertices,
                        const std::vector<uint16_t> &indices);

    const Mesh *getMesh(MeshID id) const;

  private:
    Device *device = nullptr;
    std::unordered_map<MeshID, std::unique_ptr<Mesh>> meshes;
    MeshID nextMeshId = 0;
};
