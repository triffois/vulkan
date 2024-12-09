#pragma once

#include "DescriptorLayout.h"
#include "Device.h"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class Pipeline {
  public:
    Pipeline(const std::string &vertShaderPath,
             const std::string &fragShaderPath);

    void init(Device *device, VkFormat colorFormat, VkFormat depthFormat,
              uint32_t maxFramesInFlight);

    void cleanup();

    VkPipeline getPipeline() const { return graphicsPipeline; }
    VkPipelineLayout getLayout() const { return pipelineLayout; }
    DescriptorLayout &getDescriptorLayout() { return descriptorLayout; }

    void prepareForRendering() const;

  private:
    Device *device;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;

    DescriptorLayout descriptorLayout;

    std::string vertShaderPath;
    std::string fragShaderPath;

    VkShaderModule createShaderModule(const std::vector<char> &code);
    std::vector<char> readFile(const std::string &filename);
};
