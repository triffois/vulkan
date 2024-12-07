#pragma once

#include <vulkan/vulkan.h>

class DescriptorPool {
  public:
    void init(VkDevice device, uint32_t maxSets, uint32_t maxNumStaticLightsSets);
    void cleanup();
    VkDescriptorPool getPool() const { return descriptorPool; }

  private:
    VkDevice device;
    VkDescriptorPool descriptorPool;
};
