#pragma once

#include "Buffer.h"
#include "DescriptorSet.h"
#include "GlobalResources.h"
#include "IAttachment.h"
#include <functional>
#include <memory>
#include <vector>

template <typename T> class UniformAttachment : public IAttachment {
  public:
    UniformAttachment(Device *device, std::function<void(T &)> updator,
                      uint32_t bindingLocation)
        : updator(std::move(updator)), bindingLocation(bindingLocation) {
        auto maxFramesInFlight = device->getMaxFramesInFlight();

        VkDeviceSize bufferSize = sizeof(T);
        uniformBuffers.resize(maxFramesInFlight);
        uniformBuffersMapped.resize(maxFramesInFlight);

        for (size_t i = 0; i < maxFramesInFlight; i++) {
            uniformBuffers[i] = std::make_unique<Buffer>(
                device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                VMA_MEMORY_USAGE_AUTO,
                VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

            uniformBuffers[i]->map(&uniformBuffersMapped[i]);
        }
    }

    ~UniformAttachment() override = default;

    void update(uint32_t currentFrame) override {
        T ubo{};
        updator(ubo);
        memcpy(uniformBuffersMapped[currentFrame], &ubo, sizeof(T));
    }

    void updateDescriptorSet(uint32_t maxFramesInFlight,
                             DescriptorSet &descriptorSet) override {
        for (size_t i = 0; i < maxFramesInFlight; i++) {
            descriptorSet.updateBufferInfo(i, bindingLocation,
                                           uniformBuffers[i]->getBuffer(), 0,
                                           sizeof(T));
        }
    }

    VkDescriptorSetLayoutBinding layoutBinding() const override {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = bindingLocation;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags =
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        return uboLayoutBinding;
    }

  private:
    uint32_t bindingLocation;
    std::function<void(T &)> updator;

    // Uniform buffer resources
    std::vector<std::unique_ptr<Buffer>> uniformBuffers;
    std::vector<void *> uniformBuffersMapped;
};
