#pragma once

#include "Buffer.h"
#include "Camera.h"
#include "DescriptorLayout.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "Device.h"
#include "Model.h"
#include "Image.h"
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

class Pipeline {
  public:
    Pipeline(Device *device, const std::string &vertShaderPath,
             const std::string &fragShaderPath, VkFormat colorFormat,
             VkFormat depthFormat, const Model &model,
             uint32_t maxFramesInFlight);

    void cleanup();
    void updateUniformBuffer(uint32_t currentFrame, Camera &camera,
                             const VkExtent2D &swapChainExtent) const;

    VkPipeline getPipeline() const { return graphicsPipeline; }
    VkPipelineLayout getLayout() const { return pipelineLayout; }
    const Model &getModel() const { return model; }

    VkDescriptorSet getDescriptorSet(uint32_t frameIndex) const {
        return descriptorSet.getSet(frameIndex);
    }
    VkBuffer getVertexBuffer() const { return vertexBuffer->getBuffer(); }
    VkBuffer getIndexBuffer() const { return indexBuffer->getBuffer(); }
    uint32_t getIndexCount() const { return model.getIndices().size(); }

  private:
    Device *device;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    Model model;
    Image image;

    VkSampler textureSampler;

    std::unique_ptr<Buffer> vertexBuffer;
    std::unique_ptr<Buffer> indexBuffer;
    DeviceMemoryAllocationHandle vertexBufferAllocation;
    DeviceMemoryAllocationHandle indexBufferAllocation;

    void createVertexBuffer();
    void createIndexBuffer();
    DeviceMemoryAllocationHandle createBuffer(VkDeviceSize size,
                                              VkBufferUsageFlags usage,
                                              VkMemoryPropertyFlags properties,
                                              VkBuffer &buffer);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    DescriptorLayout descriptorLayout;
    DescriptorPool descriptorPool;
    DescriptorSet descriptorSet;

    std::vector<std::unique_ptr<Buffer>> uniformBuffers;
    std::vector<void *> uniformBuffersMapped;
    std::vector<DeviceMemoryAllocationHandle> uniformBuffersAllocations;

    glm::mat4 calculateModelMatrix() const;

    void createUniformBuffers(uint32_t maxFramesInFlight);
    VkShaderModule createShaderModule(const std::vector<char> &code);
    std::vector<char> readFile(const std::string &filename);

    void createTextureResources();
    void createTextureSampler();
};
