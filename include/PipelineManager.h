#pragma once

#include "Device.h"
#include "Pipeline.h"
#include "PipelineSettings.h"
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

    PipelineID createPipeline(DescriptorLayout descriptorLayout,
                              VkFormat colorFormat, VkFormat depthFormat,
                              uint32_t maxFramesInFlight,
                              PipelineSettings &settings);

    Pipeline &getPipeline(const PipelineID &id) const;

  private:
    Device *device = nullptr;
    std::unordered_map<PipelineID, std::unique_ptr<Pipeline>> pipelines;
};
