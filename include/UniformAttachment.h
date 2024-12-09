#pragma once

#include "Buffer.h"
#include "Camera.h"
#include "DescriptorSet.h"
#include "GlobalResources.h"
#include "IAttachment.h"
#include <memory>
#include <vector>

class UniformAttachment : public IAttachment {
  public:
    UniformAttachment(GlobalResources *globalResources, Camera &camera,
                      uint32_t bindingLocation);
    ~UniformAttachment() override = default;

    void init(uint32_t maxFramesInFlight);

    // Implement interface methods
    VkDescriptorSetLayoutBinding layoutBinding() const override;
    void updateDescriptorSet(uint32_t maxFramesInFlight,
                             DescriptorSet &descriptorSet) override;
    void update(uint32_t frameIndex) override;

  private:
    GlobalResources *globalResources;
    uint32_t bindingLocation;
    Camera *camera;

    // Uniform buffer resources
    std::vector<std::unique_ptr<Buffer>> uniformBuffers;
    std::vector<void *> uniformBuffersMapped;
};
