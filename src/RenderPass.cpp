#include "RenderPass.h"
#include "Buffer.h"
#include "InstanceDataBuilder.h"
#include <cstring>

RenderPass::RenderPass(GlobalResources *globalResources,
                       const RenderBatch &batch, uint32_t maxFramesInFlight)
    : globalResources(globalResources), meshId(batch.meshId),
      pipelineId(batch.pipelineId) {

    createInstanceBuffer(batch);
    createUniformBuffers(maxFramesInFlight);
    auto device = globalResources->getDevice();
    descriptorPool.init(*device->getDevice(), maxFramesInFlight);
    descriptorSet.init(*device->getDevice(), descriptorPool,
                       device->getDescriptorLayout(), maxFramesInFlight);
    updateDescriptors(maxFramesInFlight);
}

RenderPass::~RenderPass() {
    // TODO
}

void RenderPass::createInstanceBuffer(const RenderBatch &batch) {
    std::vector<InstanceData> instanceData = buildInstanceData(batch.instances);
    instanceCount = static_cast<uint32_t>(instanceData.size());
    VkDeviceSize bufferSize = sizeof(InstanceData) * instanceData.size();

    Buffer stagingBuffer(
        globalResources->getDevice(), bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        VMA_MEMORY_USAGE_AUTO,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

    void *data;
    stagingBuffer.map(&data);
    memcpy(data, instanceData.data(), (size_t)bufferSize);
    stagingBuffer.unmap();

    instanceBuffer = std::make_unique<Buffer>(
        globalResources->getDevice(), bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

    instanceBuffer->copyFrom(stagingBuffer, bufferSize);
}

void RenderPass::createUniformBuffers(uint32_t maxFramesInFlight) {
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

void RenderPass::updateDescriptors(uint32_t maxFramesInFlight) {
    // Update uniform buffer descriptors
    for (size_t i = 0; i < maxFramesInFlight; i++) {
        descriptorSet.updateBufferInfo(0, uniformBuffers[i]->getBuffer(), 0,
                                       sizeof(UniformBufferObject));
    }

    // Update texture descriptors
    descriptorSet.updateImageInfo(
        1, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        globalResources->getTextureManager().getTextureArrayView(),
        globalResources->getTextureManager().getSampler());
}

void RenderPass::updateUniformBuffer(uint32_t currentFrame,
                                     const Camera &camera,
                                     const VkExtent2D &swapChainExtent) {
    UniformBufferObject ubo{};
    ubo.view = camera.GetViewMatrix();
    ubo.proj = glm::perspective(
        glm::radians(camera.getZoom()),
        swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 1000.0f);
    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));
}
