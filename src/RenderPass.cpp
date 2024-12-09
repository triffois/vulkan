#include "RenderPass.h"
#include "Buffer.h"
#include "InstanceDataBuilder.h"
#include <cstring>

RenderPass::RenderPass(GlobalResources *globalResources,
                       const RenderBatch &batch, PipelineID pipelineId,
                       uint32_t maxFramesInFlight)
    : globalResources(globalResources), meshId(batch.meshId),
      pipelineId(pipelineId) {
    createInstanceBuffer(batch);
    auto device = globalResources->getDevice();
    auto &pipeline =
            globalResources->getPipelineManager().getPipeline(pipelineId);

    std::unordered_map<VkDescriptorType, uint32_t> descriptorTypeCounts;
    for (auto &attachment: pipeline.getAttachments()) {
        descriptorTypeCounts[attachment.get().getType()] +=
                attachment.get().getDescriptorCount();
    }

    descriptorPool.init(*device->getDevice(), descriptorTypeCounts,
                        maxFramesInFlight);

    descriptorSet.init(*device->getDevice(), descriptorPool,
                       pipeline.getDescriptorLayout(), maxFramesInFlight);

    for (auto &attachment: pipeline.getAttachments()) {
        attachment.get().updateDescriptorSet(maxFramesInFlight, descriptorSet);
    }
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
    memcpy(data, instanceData.data(), (size_t) bufferSize);
    stagingBuffer.unmap();

    instanceBuffer = std::make_unique<Buffer>(
        globalResources->getDevice(), bufferSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

    instanceBuffer->copyFrom(stagingBuffer, bufferSize);
}

void RenderPass::update(uint32_t currentFrame) {
    for (auto &attachment: globalResources->getPipelineManager()
         .getPipeline(pipelineId)
         .getAttachments()) {
        attachment.get().update(currentFrame);
    }
}
