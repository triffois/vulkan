#pragma once

#include "Buffer.h"
#include "Camera.h"
#include "DescriptorSet.h"
#include "GlobalResources.h"
#include <memory>
#include <vector>

class UniformAttachment {
  public:
    UniformAttachment(GlobalResources *globalResources);

    void init(uint32_t maxFramesInFlight);
    void updateDescriptorSet(uint32_t maxFramesInFlight,
                             DescriptorSet &descriptorSet);
    void update(uint32_t currentFrame, const Camera &camera,
                const VkExtent2D &swapChainExtent);

  private:
    GlobalResources *globalResources;

    // Uniform buffer resources
    std::vector<std::unique_ptr<Buffer>> uniformBuffers;
    std::vector<void *> uniformBuffersMapped;
};
