#pragma once
#include "DescriptorLayout.h"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class Pipeline {
  public:
    void init(VkDevice device, const std::string &vertShaderPath,
              const std::string &fragShaderPath,
              const DescriptorLayout &descriptorLayout, VkFormat colorFormat,
              VkFormat depthFormat);

    void cleanup();

    VkPipeline getPipeline() const { return graphicsPipeline; }
    VkPipelineLayout getLayout() const { return pipelineLayout; }

  private:
    VkDevice device;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;

    VkShaderModule createShaderModule(const std::vector<char> &code);
    std::vector<char> readFile(const std::string &filename);
};
