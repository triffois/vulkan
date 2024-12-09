#pragma once

#include "Buffer.h"
#include "Camera.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "GlobalResources.h"
#include "MeshManager.h"
#include "PipelineManager.h"
#include "RenderBatch.h"
#include "UniformAttachment.h"
#include <memory>

class RenderPass {
  public:
    RenderPass(GlobalResources *globalResources, const RenderBatch &batch,
               uint32_t maxFramesInFlight, Camera &camera);
    ~RenderPass();

    // Prevent copying
    RenderPass(const RenderPass &) = delete;
    RenderPass &operator=(const RenderPass &) = delete;

    // Allow moving
    RenderPass(RenderPass &&) = default;
    RenderPass &operator=(RenderPass &&) = default;

    // Getters
    MeshID getMeshId() const { return meshId; }
    PipelineID getPipelineId() const { return pipelineId; }
    uint32_t getInstanceCount() const { return instanceCount; }
    VkBuffer getInstanceBuffer() const { return instanceBuffer->getBuffer(); }
    VkDescriptorSet getDescriptorSet(uint32_t frameIndex) const {
        return descriptorSet.getSet(frameIndex);
    }

    void update(uint32_t currentFrame, const Camera &camera,
                const VkExtent2D &swapChainExtent);

  private:
    void createInstanceBuffer(const RenderBatch &batch);
    void updateDescriptors(uint32_t maxFramesInFlight);

    std::unique_ptr<UniformAttachment> uniformAttachment;

    GlobalResources *globalResources;
    MeshID meshId;
    PipelineID pipelineId;
    std::unique_ptr<Buffer> instanceBuffer;
    uint32_t instanceCount;

    // Descriptor resources
    DescriptorPool descriptorPool;
    DescriptorSet descriptorSet;
};
