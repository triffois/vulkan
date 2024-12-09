#pragma once

#include "DescriptorLayout.h"
#include "DescriptorPool.h"
#include <vector>
#include <vulkan/vulkan.h>

class DescriptorSet {
  public:
    void init(VkDevice device, const DescriptorPool &pool,
              const DescriptorLayout &layout, uint32_t count);

    void updateBufferInfo(size_t frameIndex, uint32_t binding, VkBuffer buffer,
                          VkDeviceSize offset, VkDeviceSize range);

    void updateImageInfos(uint32_t startBinding,
                          const std::vector<VkImageView> &views,
                          const std::vector<VkImageLayout> &layouts,
                          VkSampler sampler);

    void updateImageInfo(uint32_t binding, VkImageLayout layout,
                         VkImageView view, VkSampler sampler);

    VkDescriptorSet getSet(uint32_t index) const;

  private:
    VkDevice device;
    std::vector<VkDescriptorSet> descriptorSets;
};
