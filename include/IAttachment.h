#pragma once

#include "DescriptorSet.h"

// An attachment to the pipeline - gets bound to an index in the descriptor set
class IAttachment {
  public:
    virtual ~IAttachment() = default;

    virtual VkDescriptorSetLayoutBinding layoutBinding() const = 0;
    virtual void updateDescriptorSet(uint32_t maxFramesInFlight,
                                     DescriptorSet &descriptorSet) = 0;
    virtual void update(uint32_t frameIndex) = 0;
};
