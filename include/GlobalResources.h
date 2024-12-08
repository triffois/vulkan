#pragma once

#include "Device.h"
#include "MeshManager.h"
#include "PipelineManager.h"
#include "TextureManager.h"

class GlobalResources {
  public:
    GlobalResources() = default;
    ~GlobalResources() = default;

    void init(Device *device);

    void cleanup();

    PipelineManager &getPipelineManager() { return pipelineManager; }
    MeshManager &getMeshManager() { return meshManager; }
    TextureManager &getTextureManager() { return textureManager; }
    Device *getDevice() { return device; }

    void prepareResources(VkFormat colorFormat, VkFormat depthFormat);

  private:
    Device *device = nullptr;
    PipelineManager pipelineManager;
    MeshManager meshManager;
    TextureManager textureManager;
};
