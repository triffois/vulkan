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

    PipelineID createPipeline(
        DescriptorLayout descriptorLayout, VkFormat colorFormat,
        VkFormat depthFormat, uint32_t maxFramesInFlight,
        const std::string &vertPath, const std::string &fragPath,
        std::vector<std::reference_wrapper<IAttachment>> attachments);

    Pipeline &getPipeline(const PipelineID &id) const;

  private:
    Device *device = nullptr;
    std::unordered_map<PipelineID, std::unique_ptr<Pipeline>> pipelines;
};
