#pragma once

#include <unordered_map>
#include <vulkan/vulkan.h>

class DescriptorPool {
  public:
    void
    init(VkDevice device,
         std::unordered_map<VkDescriptorType, uint32_t> &descriptorTypeCounts,
         uint32_t maxSets);
    void cleanup();
    VkDescriptorPool getPool() const { return descriptorPool; }

  private:
    VkDevice device;
    VkDescriptorPool descriptorPool;
};
