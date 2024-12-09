#pragma once

#include "DescriptorLayout.h"
#include "Device.h"
#include "IAttachment.h"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class Pipeline {
  public:
    Pipeline(Device *device, DescriptorLayout descriptorLayout,
             VkFormat colorFormat, VkFormat depthFormat,
             uint32_t maxFramesInFlight, const std::string &vertShaderPath,
             const std::string &fragShaderPath,
             std::vector<std::reference_wrapper<IAttachment>> attachments);

    // Make uncopyable
    Pipeline(const Pipeline &) = delete;
    Pipeline &operator=(const Pipeline &) = delete;

    void cleanup();

    VkPipeline getPipeline() const { return graphicsPipeline; }
    VkPipelineLayout getLayout() const { return pipelineLayout; }
    const std::vector<std::reference_wrapper<IAttachment>> &
    getAttachments() const {
        return attachments;
    }

    void bind(IAttachment &attachment) { attachments.push_back(attachment); }
    DescriptorLayout &getDescriptorLayout() { return descriptorLayout; }

    void prepareForRendering() const;

  private:
    Device *device;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    DescriptorLayout descriptorLayout;

    std::vector<std::reference_wrapper<IAttachment>> attachments;

    VkShaderModule createShaderModule(const std::vector<char> &code);
    std::vector<char> readFile(const std::string &filename);
};
