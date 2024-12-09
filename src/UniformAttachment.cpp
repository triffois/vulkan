#include "UniformAttachment.h"

UniformAttachment::UniformAttachment(GlobalResources *globalResources,
                                     Camera &camera, uint32_t bindingLocation)
    : globalResources(globalResources), camera(&camera),
      bindingLocation(bindingLocation) {}

void UniformAttachment::init(uint32_t maxFramesInFlight) {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    uniformBuffers.resize(maxFramesInFlight);
    uniformBuffersMapped.resize(maxFramesInFlight);

    for (size_t i = 0; i < maxFramesInFlight; i++) {
        uniformBuffers[i] = std::make_unique<Buffer>(
            globalResources->getDevice(), bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            VMA_MEMORY_USAGE_AUTO,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

        uniformBuffers[i]->map(&uniformBuffersMapped[i]);
    }
}

void UniformAttachment::update(uint32_t currentFrame) {
    auto &swapChain = globalResources->getSwapChain();
    VkExtent2D swapChainExtent = swapChain.getExtent();

    UniformBufferObject ubo{};
    ubo.view = camera->GetViewMatrix();
    ubo.proj = glm::perspective(
        glm::radians(camera->getZoom()),
        swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 1000.0f);
    ubo.proj[1][1] *= -1;

    // Get texture resolutions and fill the array
    auto resolutions =
        globalResources->getTextureManager().getTextureResolutions();

    // Fill remaining slots with zero
    for (size_t i = resolutions.size(); i < 256; i++) {
        ubo.textureResolutions[i] = glm::vec4(0.0f);
    }
    for (size_t i = 0; i < resolutions.size(); i++) {
        ubo.textureResolutions[i] = resolutions[i];
    }

    memcpy(uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));
}

void UniformAttachment::updateDescriptorSet(uint32_t maxFramesInFlight,
                                            DescriptorSet &descriptorSet) {
    for (size_t i = 0; i < maxFramesInFlight; i++) {
        descriptorSet.updateBufferInfo(i, 0, uniformBuffers[i]->getBuffer(), 0,
                                       sizeof(UniformBufferObject));
    }
}

VkDescriptorSetLayoutBinding UniformAttachment::layoutBinding() const {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = bindingLocation;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    return uboLayoutBinding;
}
