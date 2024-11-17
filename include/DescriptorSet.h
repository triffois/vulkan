#pragma once

#include "DescriptorLayout.h"
#include "DescriptorPool.h"
#include <vector>
#include <vulkan/vulkan.h>

class DescriptorSet {
  public:
    void init(VkDevice device, const DescriptorPool &pool,
              const DescriptorLayout &layout, uint32_t count);

    void updateBufferInfo(uint32_t binding, VkBuffer buffer,
                          VkDeviceSize offset, VkDeviceSize range);

    void updateImageInfo(uint32_t binding, VkImageLayout layout,
                         VkImageView view, VkSampler sampler);

    VkDescriptorSet getSet(uint32_t index) const;

  private:
    VkDevice device;
    std::vector<VkDescriptorSet> descriptorSets;
};
