#pragma once

#include "Camera.h"
#include "DescriptorLayout.h"
#include "DescriptorPool.h"
#include "DescriptorSet.h"
#include "Device.h"
#include "Model.h"
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
    VkDescriptorSet getDescriptorSet(uint32_t frameIndex) const;

    VkBuffer getVertexBuffer() const { return vertexBuffer; }
    VkBuffer getIndexBuffer() const { return indexBuffer; }
    uint32_t getIndexCount() const { return model.getIndices().size(); }

  private:
    Device *device;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    Model model;

    VkBuffer vertexBuffer; // TODO: use own abstraction
    VkBuffer indexBuffer;
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

    std::vector<VkBuffer> uniformBuffers;
    std::vector<void *> uniformBuffersMapped;
    std::vector<DeviceMemoryAllocationHandle> uniformBuffersAllocations;

    glm::mat4 calculateModelMatrix() const;

    void createUniformBuffers(uint32_t maxFramesInFlight);
    VkShaderModule createShaderModule(const std::vector<char> &code);
    std::vector<char> readFile(const std::string &filename);
};
