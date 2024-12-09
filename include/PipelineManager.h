#pragma once

#include "Device.h"
#include "Pipeline.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vulkan/vulkan.h>

using PipelineID = std::string;

class PipelineManager {
  public:
    PipelineManager() = default;
    ~PipelineManager() = default;

    PipelineManager(const PipelineManager &) = delete;
    PipelineManager &operator=(const PipelineManager &) = delete;

    void init(Device *device);
    void cleanup();

    PipelineID createPipeline(const std::string &vertPath,
                              const std::string &fragPath);

    Pipeline &getPipeline(const PipelineID &id) const;

    // Should be called after all the textures have been loaded
    // I don't know why - to eepy to understand my own code at this point -
    // but it's necessary
    void prepareResources(VkFormat colorFormat, VkFormat depthFormat);

  private:
    Device *device = nullptr;
    std::unordered_map<PipelineID, std::unique_ptr<Pipeline>> pipelines;
};
