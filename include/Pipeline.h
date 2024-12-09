#pragma once

#include "DescriptorLayout.h"
#include "Device.h"
#include "IAttachment.h"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class Pipeline {
  public:
    Pipeline(const std::string &vertShaderPath,
             const std::string &fragShaderPath);

    // Make uncopyable
    Pipeline(const Pipeline &) = delete;
    Pipeline &operator=(const Pipeline &) = delete;

    void init(Device *device, VkFormat colorFormat, VkFormat depthFormat,
              uint32_t maxFramesInFlight);

    void cleanup();

    VkPipeline getPipeline() const { return graphicsPipeline; }
    VkPipelineLayout getLayout() const { return pipelineLayout; }
    const std::vector<std::reference_wrapper<IAttachment>> &
    getAttachments() const {
        return attachments;
    }
    DescriptorLayout &getDescriptorLayout() { return descriptorLayout; }

    void bind(IAttachment &attachment) { attachments.push_back(attachment); }

    void prepareForRendering() const;

  private:
    Device *device;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;

    std::vector<std::reference_wrapper<IAttachment>> attachments;
    DescriptorLayout descriptorLayout;

    std::string vertShaderPath;
    std::string fragShaderPath;

    VkShaderModule createShaderModule(const std::vector<char> &code);
    std::vector<char> readFile(const std::string &filename);
};
