#pragma once

#include "Buffer.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "GlobalResources.h"
#include "MeshManager.h"
#include "PipelineManager.h"
#include "RenderBatch.h"
#include <memory>

class RenderPass {
  public:
    RenderPass(GlobalResources *globalResources, const RenderBatch &batch,
               PipelineID pipelineId, uint32_t maxFramesInFlight);

    ~RenderPass() = default;

    void cleanUp();

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

    void update(uint32_t currentFrame);

  private:
    void createInstanceBuffer(const RenderBatch &batch);

    GlobalResources *globalResources;
    MeshID meshId;
    PipelineID pipelineId;
    std::unique_ptr<Buffer> instanceBuffer;
    uint32_t instanceCount;

    // Descriptor resources
    DescriptorPool descriptorPool;
    DescriptorSet descriptorSet;
};
