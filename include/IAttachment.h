#pragma once

#include "DescriptorSet.h"
#include <vulkan/vulkan.h>

// An attachment to the pipeline - gets bound to an index in the descriptor set
class IAttachment {
  public:
    virtual ~IAttachment() = default;

    virtual VkDescriptorSetLayoutBinding layoutBinding() const = 0;

    virtual void updateDescriptorSet(uint32_t maxFramesInFlight,
                                     DescriptorSet &descriptorSet) = 0;

    virtual void update(uint32_t frameIndex) = 0;

    virtual VkDescriptorType getType() const = 0;

    virtual uint32_t getDescriptorCount() const { return 1; }
};
